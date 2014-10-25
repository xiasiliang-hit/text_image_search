<?php
function format($n){
if($_SESSION['$tick']==0){
	if($n==3)
		$input = shell_exec("mpstat -P ALL 1 1  &> /dev/null &"); //  &> /dev/null &
	else{
		//$input = shell_exec("ssh node$n \"mpstat -P ALL 1 1 2>&1\"  &> /dev/null &");
	}
	file_put_contents("log/cpu_usage_$n.txt",$input);
}else
	$input = file_get_contents("log/cpu_usage_$n.txt");

$lines = explode("\n",$input);
$cap = "(node$n) 12 CPU Linux x64";
$out = "<table border=\"0\"  width=\"300\">"."\n<tr><th>CPU</th><th>USER</th><th>SYSTEM</th><th>IDLE</th>"."</tr>";
$out .= "<caption align=\"center\">$cap</caption>";
if($n==3)
for($i=3;$i<16;$i++){
	$line = preg_split('/[\s]+/',$lines[$i]);
	if(sizeof($line)>12)
		$out .= "\n<tr align=\"center\"><td>".$line[2]." </td><td> ".$line[3]." </td><td> ".$line[4]." </td><td> ".$line[12]." </td>"."</tr>";
	else
		$out .= "\n<tr align=\"center\"><td>".$line[1]." </td><td> ".$line[2]." </td><td> ".$line[3]." </td><td> ".$line[11]." </td>"."</tr>";
}
else
for($i=3;$i<16;$i++){
	$out .= "\n<tr align=\"center\"><td>"."N/A"." </td><td> "."N/A"." </td><td> "."N/A"." </td><td> "."N/A"." </td>"."</tr>";
}
//machine status.
$a = explode("\n",file_get_contents("log/machine_status_$n.txt"));
for($i=0;$i<sizeof($a);$i++){
	$arr = explode(":",$a[$i]);
	if(strlen($arr[0])>0)
		$out .= "\n<tr><td colspan=\"2\" align=\"left\">".$arr[0].": </td><td colspan=\"2\"> ".$arr[1]." </td>"."</tr>";
}

$out .= "\n</table>";
return $out;
}
if (!isset($_SESSION))
	session_start();

$r1 = format(1);
$r2 = format(2);
$r3 = format(3);
$out = "<hr><table  border=\"0\"  width=\"100%\"><tr><td>".$r1."</td><td>".$r2."</td><td>".$r3."</td></tr></table><hr>";

$_SESSION['$tick'] += 1;
$_SESSION['$tick'] %= 4;

//echo $out;
echo $out."\n<p><b>TERMINAL OUTPUTS</b></p>".file_get_contents("log/log.txt");
	
?>
