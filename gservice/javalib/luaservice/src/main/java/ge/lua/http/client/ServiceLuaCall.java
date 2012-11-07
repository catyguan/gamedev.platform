package ge.lua.http.client;

import ge.lua.LuaArray;
import ge.lua.LuaTable;
import ge.lua.host.LuaApp;
import ge.lua.host.LuaCallWithName;

import java.net.URL;
import java.nio.charset.Charset;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.thrift.TException;
import org.jboss.netty.buffer.ChannelBuffer;
import org.jboss.netty.buffer.ChannelBuffers;
import org.jboss.netty.channel.Channel;
import org.jboss.netty.handler.codec.http.DefaultHttpRequest;
import org.jboss.netty.handler.codec.http.HttpChunkAggregator;
import org.jboss.netty.handler.codec.http.HttpClientCodec;
import org.jboss.netty.handler.codec.http.HttpContentDecompressor;
import org.jboss.netty.handler.codec.http.HttpHeaders;
import org.jboss.netty.handler.codec.http.HttpMethod;
import org.jboss.netty.handler.codec.http.HttpRequest;
import org.jboss.netty.handler.codec.http.HttpResponse;
import org.jboss.netty.handler.codec.http.HttpVersion;
import org.jboss.netty.util.CharsetUtil;

import bma.common.langutil.ai.stack.AIStack;
import bma.common.langutil.ai.stack.AIStackROOT;
import bma.common.langutil.ai.stack.AIStackStep;
import bma.common.langutil.core.ExceptionUtil;
import bma.common.langutil.core.SizeUtil;
import bma.common.langutil.core.SizeUtil.Unit;
import bma.common.langutil.core.ValueUtil;
import bma.common.langutil.http.QueryStringEncoder;
import bma.common.langutil.io.HostPort;
import bma.common.netty.NettyUtil;
import bma.common.netty.client.CommonNettyClientSender;
import bma.common.netty.client.NettyClient;
import bma.common.netty.pool.NettyChannelPool;

public class ServiceLuaCall implements LuaCallWithName {

	final org.slf4j.Logger log = org.slf4j.LoggerFactory
			.getLogger(ServiceLuaCall.class);

	public static final String METHOD_NAME = "http.client.execute";

	@Override
	public String getName() {
		return METHOD_NAME;
	}

	protected NettyChannelPool pool;
	protected int maxContentLength = 10 * 1024 * 1024;

	public ServiceLuaCall() {
		super();
	}

	public int getMaxContentLength() {
		return maxContentLength;
	}

	public void setMaxContentLength(int maxContentLength) {
		this.maxContentLength = maxContentLength;
	}

	public void setMaxContentLengthValue(String s) {
		try {
			this.maxContentLength = (int) SizeUtil.convert(s, 1024, Unit.B);
		} catch (Exception e) {
			throw ExceptionUtil.throwRuntime(e);
		}
	}

	public NettyChannelPool getPool() {
		return pool;
	}

	public void setPool(NettyChannelPool pool) {
		this.pool = pool;
	}

	@Override
	public boolean call(final LuaApp app, final int callId, LuaArray data)
			throws Exception {
		AIStackROOT<LuaArray> root = new AIStackROOT<LuaArray>() {

			@Override
			public boolean end(LuaArray result, Throwable t) {
				if (t != null && log.isDebugEnabled()) {
					log.debug("HttpServiceLuaCall fail", t);
				}
				if (app != null) {
					app.luaCallResponse(callId, result,
							t == null ? null : t.getMessage());
				} else {
					if (log.isDebugEnabled()) {
						log.debug("app is null,discard {}/{}", result, t);
					}
				}
				return true;
			}
		};

		String url = data.getString(0);
		if (ValueUtil.empty(url)) {
			throw new IllegalArgumentException("url invalid");
		}
		LuaTable headers = data.getTable(1);
		LuaTable params = data.getTable(2);
		String content = data.getString(3);
		LuaTable options = data.getTable(4);
		return invoke(root, url, headers, params, content, options);
	}

	public boolean invoke(AIStack<LuaArray> stack, String url,
			final LuaTable headers, final LuaTable params,
			final String content, final LuaTable options) throws TException {

		URL tmpObj;
		try {
			tmpObj = new URL(url);
		} catch (Exception e) {
			throw new TException(e);
		}
		final URL urlObj = tmpObj;

		AIStackStep<Channel, LuaArray> step1 = new AIStackStep<Channel, LuaArray>(
				stack) {

			@Override
			protected boolean next(Channel result) {
				NettyClient client = NettyClient.bind(result);

				client.send(new CommonNettyClientSender<LuaArray>(delegate()) {

					@Override
					public void bindNettyClient(NettyClient client, boolean bind) {
						if (bind) {
							client.addHandler("codec", new HttpClientCodec());
							client.addHandler("chunkded",
									new HttpChunkAggregator(maxContentLength));
							client.addHandler("inflater",
									new HttpContentDecompressor());
						} else {
							client.removeHandler("codec");
							client.removeHandler("chunkded");
							client.removeHandler("inflater");
						}
					}

					@Override
					public boolean messageReceived(NettyClient client,
							Object message) {
						if (message instanceof HttpResponse) {
							HttpResponse response = (HttpResponse) message;

							LuaArray r = new LuaArray();
							LuaTable t = new LuaTable();
							r.addTable(t);

							t.putAt("status", response.getStatus().getCode());
							List<Entry<String, String>> hs = response
									.getHeaders();
							if (hs != null && !hs.isEmpty()) {
								LuaTable ths = new LuaTable();
								for (Entry<String, String> e : hs) {
									ths.putAt(e.getKey(), e.getValue());
								}
								t.putAt("headers", ths);
							}
							ChannelBuffer content = response.getContent();
							Charset ch = NettyUtil.getContentType(response);
							String str = content.toString(ch);
							t.putAt("content", str);

							end(client, r, null);
							return true;
						}
						return false;
					}

					@Override
					public void sendRequest(NettyClient client) {
						LuaTable opts = LuaTable.checkNull(options);
						LuaTable hs = LuaTable.checkNull(headers);
						try {
							String ps = null;
							if (params != null) {
								Map<String, Object> m = params.getData();
								if (!m.isEmpty()) {
									QueryStringEncoder builder = new QueryStringEncoder(
											"");
									for (Map.Entry<String, Object> e : m
											.entrySet()) {
										Object v = e.getValue();
										builder.addParam(e.getKey(),
												v.toString());
									}
									StringBuilder sb = new StringBuilder();
									builder.buildParams(sb);
									ps = sb.toString();
								}
							}
							HttpVersion version = HttpVersion.valueOf(opts
									.getString("version", "HTTP/1.1"));
							HttpMethod method = HttpMethod.valueOf(opts
									.getString("method", "get"));
							boolean post = HttpMethod.POST.equals(method);
							String rawpath = urlObj.toURI().getRawPath();
							if (rawpath == null || rawpath.isEmpty()) {
								rawpath = "/";
							}
							StringBuilder vurl = new StringBuilder(rawpath);
							if (!post || content != null) {
								if (vurl.indexOf("?") != -1) {
									vurl.append('?').append(ps);
								} else {
									if (vurl.charAt(vurl.length() - 1) != '&') {
										vurl.append('&');
									}
									vurl.append(ps);
								}
							}

							HttpRequest request = new DefaultHttpRequest(
									version, method, vurl.toString());
							for (Map.Entry<String, Object> e : hs.getData()
									.entrySet()) {
								Object v = e.getValue();
								if (v != null) {
									request.setHeader(e.getKey(), v);
								}
							}
							if (request.getHeader(HttpHeaders.Names.HOST) == null) {
								StringBuilder host = new StringBuilder(urlObj
										.getHost());
								if (urlObj.getPort() != -1) {
									host.append(":");
									host.append(urlObj.getPort());
								}
								request.setHeader(HttpHeaders.Names.HOST,
										host.toString());
							}
							if (request.getHeader(HttpHeaders.Names.CONNECTION) == null) {
								request.setHeader(HttpHeaders.Names.CONNECTION,
										"Keep-Alive");
							}
							if (request
									.getHeader(HttpHeaders.Names.CONTENT_TYPE) == null) {
								request.setHeader(
										HttpHeaders.Names.CONTENT_TYPE,
										"application/x-www-form-urlencoded");
							}
							if (request.getHeader(HttpHeaders.Names.ACCEPT) == null) {
								request.setHeader(HttpHeaders.Names.ACCEPT,
										"*/*");
							}
							if (request.getHeader(HttpHeaders.Names.USER_AGENT) == null) {
								request.setHeader(HttpHeaders.Names.USER_AGENT,
										"Java/LuaHttpService");
							}
							if (post) {
								ChannelBuffer buf;
								if (content != null) {
									buf = ChannelBuffers.copiedBuffer(content,
											CharsetUtil.UTF_8);
								} else {
									buf = ChannelBuffers.copiedBuffer(ps,
											CharsetUtil.UTF_8);
								}
								request.setContent(buf);

								if (request
										.getHeader(HttpHeaders.Names.CONTENT_LENGTH) == null) {
									request.setHeader("Content-Length",
											buf.writerIndex());
								}
							}

							// Send the HTTP request.
							client.write(request);
						} catch (Exception e) {
							end(client, null, e);
						}
					}
				});
				return false;
			}
		};

		HostPort host = new HostPort();
		host.setHost(urlObj.getHost());
		host.setPort(urlObj.getPort() == -1 ? 80 : urlObj.getPort());
		pool.borrowObject(step1, host);

		return false;
	}
}
