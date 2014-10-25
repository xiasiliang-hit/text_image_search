file_content = 0;

window.onload = function() {
		var fileInput = document.getElementById('picture');
		fileInput.addEventListener('change', function(e) {
			var file = fileInput.files[0];
			var textType = /image.*/;
			if (file.type.match(textType)) {
				var reader = new FileReader();
				reader.onload = function(e) {
					file_content = reader.result;//.match(/,(.*)$/)[1];
				}
				reader.readAsDataURL(file);	
			}
		});
}


$(document).ready(function() {
$("#submit").click(function() {
document.getElementById("bd").style.cursor = 'wait';
var text = $("#text").val();
var picture = $("#picture").val();

$.post("submit.php", {
text1: text,
picture1: file_content
}, function(data){
	$('#form')[0].reset(); // To reset form fields
	document.getElementById("bd").style.cursor = 'default';
	if(data)
		alert(data);
	file_content = null;
});
});
});


$(document).ready(function() {
  $(window).keydown(function(event){
    if(event.keyCode == 13) {
      event.preventDefault();
      return false;
    }
  });
});
