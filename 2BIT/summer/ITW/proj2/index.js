$(function(){
    $('#dog').hide();
    var novh4 = $('#click');
    if($.cookie('dog') == 'T'){
        $('#dog').show();
    }
    novh4.click(function(){
        var dog = $('#dog');
        var cookie = $.cookie('dog');
        if(cookie == "F") {
            dog.show();
            $.cookie('dog', 'T');
        }
        else {
            dog.hide();
            $.cookie('dog','F');
        }
    })
});
