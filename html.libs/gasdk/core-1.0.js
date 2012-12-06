(function($) {
	var gasdk = {
			init: function(setting) {
				this.setting = $.extend({
					url: 'gaproxy.php',
					timeout: 5000,
				}, setting);				
			}			
	};
	
	$.gasdk = gasdk;
	
})(jQuery);