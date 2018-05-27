/* SPEECH RECOGNITION SETUP */
var recognition = new webkitSpeechRecognition();
recognition.continuous = true;
// recognition.interimResults = true;
console.log(recognition);
recognition.lang = "ko-KR";

/* COMPONENT */
recognition.onresult = function(event) {
    var sentence = event.results[event.results.length-1][0].transcript.trim();
    var url = 'https://westus.api.cognitive.microsoft.com/luis/v2.0/apps/32f607a4-d5db-420d-8676-2d97d034dd92?subscription-key=8eb8c0a2793f41bea76e8fa347684af7&verbose=true&timezoneOffset=0&q=';
    $('.text').removeClass('fadeInUp');
    console.log(sentence);

    $.ajax({
      type: 'GET',
      url: url + encodeURIComponent(sentence),
      success: function(res) {
        console.log(res);
        console.log(res.query);
  
  
        switch (res.topScoringIntent.intent) {
          case "turn_on":
            $('.intent').text("turn on the light");
            break;
          case "turn_off":
            $('.intent').text("turn off the light");
            break;
        }
        $('.intent').append("<br/>"+"("+(res.topScoringIntent.score*100).toFixed(2)+"%)")
        if (res.entities.length === 0) {
          $('.text').addClass('fadeInUp');
          $('.text').text(sentence);
        }
        res.entities.forEach(function(item) {
          console.log(item);
          var txt = sentence;
          console.log('txt : '+txt);
          var highlight = $('<b/>').text(txt.slice(item.startIndex, item.endIndex+1));
          highlight.addClass('highlight');
          $('.text').addClass('fadeInUp');
          $('.text').html(highlight);
          $('.text').append(sentence.slice(item.endIndex+1));
        })
      }
    });
  }
  recognition.start();
  