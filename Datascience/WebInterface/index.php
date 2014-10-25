<html>
<head>
<title>Text-Image Search Engine</title>
<script src="http://code.jquery.com/jquery-latest.js"></script>
<script src="refreshform.js"></script>
<style type="text/css">

	table { border-collapse:collapse; }
	table thead th { border-bottom: 1px solid #000; }
</style>
<script>
(function($)
{
    $(document).ready(function()
    {
        $.ajaxSetup(
        {
            cache: false,
            beforeSend: function() {
                //$('#content').hide();
                //$('#loading').show();
            },
            complete: function() {
                //$('#loading').hide();
                //$('#content').show();
            },
            success: function() {
                //$('#loading').hide();
                //$('#content').show();
            }
        });
        var $container = $("#content");
        $container.load("load-log.php");
        var refreshId = setInterval(function()
        {
            $container.load('load-log.php');
        }, 500);
    });
})(jQuery);

</script>

</head>

<body id = "bd">
 
<form id="form" name="form">
<p>
<input type="text" id="text" size="28"> <br>Text &nbsp|&nbsp Image &nbsp|&nbsp Text+Image <br>
<input id="submit" type="button" value="Search">
<input type="file" id="picture" accept='image/*' />
</p>
</form>
<p>
<?php
if (!isset($_SESSION)){
	session_start();
	$_SESSION['$tick']=1;
}
?>
<div id="content"></div>
</body>
</html>
