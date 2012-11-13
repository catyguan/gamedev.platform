<?php
$thriftBin = 'd:/workroom/java/thrift-0.8.0.exe';
$thriftFile = 'D:/workroom/gamedev.platform/gservice/javalib/luaservice/docs/LuaAppHostManager.thrift';
$targetPath = 'D:/workroom/gamedev.platform/gservice/phplib/modules/ge/lua/service/thrift';
$clearTarget = true;

function handleFiles($dir,$fun) {
    $dh = opendir($dir);
    while(($f = readdir($dh)) !== false) {
        if($f != '.' && $f != '..') {
            call_user_func($fun,$dir,$f);
        }
    }
}

function delete_file($dir,$f) {
    $fn = $dir.'/'.$f;
    if(is_dir($fn)) {
        handleFiles($fn,'delete_file');
        @rmdir($fn);
    } else {        
        @unlink($fn);
    }
}

function x_file($dir,$f) {
    $fn = $dir.'/'.$f;
    if(is_dir($fn)) {
        handleFiles($fn,'x_file');
        @rmdir($fn);
    } else {
        global $targetPath;
        if($dir==$targetPath) {
            return;
        }
        
        $nf = $f;
        $c = substr_count($f,'.');
        if($c==2) {
            $i1 = strpos($f,'.');
            $i2 = strrpos($f,'.');
            $nf = substr($f,$i1+1,$i2-$i1).'class.php';
            copy($fn,$targetPath.'/'.$nf);
            @unlink($fn);
            return;
        }
        if(strpos($f,'_types.php')!==false) {
            @unlink($fn);
            return;
        }
        if($c==3) {
            $i1 = strpos($f,'.');
            $i2 = strrpos($f,'.');
            $i3 = strpos($f,'.',$i1+1);
            $nf = substr($f,$i1+1,$i3-$i1-1).'Client.class.php';
        } else {
            $i2 = strrpos($f,'.');
            $nf = substr($f,0,$i2).'If.class.php';
        }
        
        if(true) {
            $str = file_get_contents($fn);
            $list = explode("\n",$str);
            $r = '';
            foreach($list as $ln) {
                if($c==3) {
                    if(strpos($ln,'unknown result')!==false) {
                        $r .= '    return $result->success; // ';
                    }                    
                } else {
                    if(strpos($ln,'GLOBALS')!==false) {
                        $r .= '// ';
                    }
                }
                $r .= $ln;
            }
            file_put_contents($targetPath.'/'.$nf,$r);                
            @unlink($fn);
            return;          
        }
    }
}

if(!file_exists($thriftFile)) {
    die("$thriftFile not exists");
}
if(!file_exists($targetPath)) {
    die("$targetPath not exists");
}

if($clearTarget) {    
    handleFiles($targetPath,'delete_file');
}

$re = null;
$cmd = "$thriftBin --gen php:namespace,oop,autoload -out $targetPath $thriftFile ";

system($cmd,$re);

//$fn = $dir.'/'.$f;
//            if(is_dir($fn)) {
//                handleFiles($fn);
//            } else {
//                echo "$fn\n";
                // @unlink($f);
//            }
handleFiles($targetPath,'x_file');
