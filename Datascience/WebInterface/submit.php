<?php
function send_data_to_server($data){
	$service_port = 59886;
	$address = '127.0.0.1';
	/* Create a TCP/IP socket. */
	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	if ($socket === false){
		return "Failed to create socket";
	}
	$result = socket_connect($socket, $address, $service_port);
	if ($result === false){
		return "Failed to connect to server";
	}
	$in = $data."\r\n"; //'HTTP' header is necessary.
	socket_write($socket, $in, strlen($in));
	fflush($socket);
	return null;
}

$text = $_POST['text1'];

$pic = $_POST['picture1'];//data:image/jpeg;base64,/9j/4AAQSkZJRg...

if($pic!=null){
	$pic = explode(",", $pic); //Split the data into two: pic[0] and pic[1]. Data format is "data:<MIME info>;base64,<base64 encoded string>"
	$pic[1] = str_replace(' ', '+', $pic[1]); //PHP handles spaces in base64 encoded string differently so to prevent corruption of data, convert spaces to +
	if(strlen($pic[1])>10000){
		echo "File size must be no more than 10 kb.";
		return;
	}
	$pic = base64_decode($pic[1]);
}

if($pic==null && $text!=null){ //text only
	$ret = send_data_to_server($text."\0");
}elseif($pic!=null && $text==null){ //image only
	//$im = new imagick($pic); 
	//$im->resizeImage (200, 200, imagick::FILTER_LANCZOS, 1, TRUE);
	$ret = send_data_to_server("\0".$pic);
}elseif($pic!=null && $text!=null){ //both
	//$im = new imagick($pic); 
	//$im->resizeImage ( 200, 200, imagick::FILTER_LANCZOS, 1, TRUE);
	$ret = send_data_to_server($text."\0".$pic);
}

if($ret != null)
	unlink('log.txt');	
	
echo $ret;
?>
