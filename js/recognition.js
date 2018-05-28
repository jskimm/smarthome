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
  
  /* XHR implementation */
  function xhr_open(method, path)  {
    var xhr = new XMLHttpRequest();
    xhr.open(method, 'http://192.168.43.142/'+path);
    xhr.send();
  }
  
  $.ajax({
    type: 'GET',
    url: url + encodeURIComponent(sentence),
    success: function(res) {
      console.log(res);
      console.log(res.query);

      switch (res.topScoringIntent.intent) {
        /* LIGHT ON/OFF */
        case "Red_light_off":
          xhr_open('GET', 'LR');
          $('.intent').text("빨간 불을 켰습니다.");
          break;
        case "Red_light_on":
          xhr.open('GET',host + 'HR');
          xhr.send();
          console.log("Red_light_on");
          $('.intent').text("불 커다.");
          break;

        case "Green_light_off":
          xhr.open('GET',host + 'LG');
          xhr.send();
          console.log("Green_light_off");
          break;
        case "Green_light_on":
          xhr.open('GET',host + 'HG');
          xhr.send(); 
          console.log("Green_light_on");
          $('.intent').text("불 커다.");
          break;

        case "Blue_light_off":
          xhr.open('GET',host + 'LB');
          xhr.send();
          console.log("Blue_light_off");
          break;
        case "Blue_light_on":
          xhr.open('GET',host + 'HB');
          xhr.send();
          console.log("Blue_light_on");
          $('.intent').text("불 커다.");
          break;
        
        /* TEMP HUM CHECK */ 
        case "temperature_humidity check":
          xhr.open('GET',host + 'HTEM');
          xhr.send();
          console.log("temperature_humidity check");
          $('.intent').text("옵습도 측정.");
          break;

        /* GAS CHECK */ 
        case "gas_check": 
          xhr.open('GET',host + 'HGAS');
          xhr.send();
          console.log("gas_check");
          $('.intent').text("가스 측정.");
          break;

        /* DUST CHECK */ 
        case "Dust_check":
          xhr.open('GET',host + 'HDUST');
          xhr.send();
          console.log("Dust_check");

          $('.intent').text("미세먼지 농도 측정.");
          break;
        
        /* MUSIC ON/OFF */
        case "music_off":
          xhr.open('GET',host + 'HBU');/*buser sound : 도레미파솔라시도*/
          xhr.send();
          console.log("music_off");
          $('.intent').text("음악 끄다.");
          break;
        case " music_on":
          $('.intent').text("음악 켜다.");
          break;

        /* SEARCH */
        case "none":
          $('.intent').text("검색 출력 내용.");
            break;
        case "Exception":
          $('.intent').text("")
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
  