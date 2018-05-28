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
          $('.intent').text("빨간 불을 껐습니다.");
          break;
        case "Red_light_on":
          xhr.open('GET',host + 'HR')
          $('.intent').text("빨간 불을 컸습니다.");
          break;

        case "Green_light_off":
          xhr.open('GET',host + 'LG');
          $('.intent').text("녹색 불을 껐습니다.");
          break;
        case "Green_light_on":
          xhr.open('GET',host + 'HG');
          $('.intent').text("녹색 불을 컸습니다.");
          break;
        case "Blue_light_off":
          xhr.open('GET',host + 'LB');
          $('.intent').text("파랑 불을 껐습니다.");
          break;
        case "Blue_light_on":
          xhr.open('GET',host + 'HB');
          $('.intent').text("파랑 불을 컸습니다.");
          break;
        
        /* TEMP HUM CHECK */ 
        case "temperature_humidity check":
          xhr.open('GET',host + 'HTEM');
          $('.intent').text("현재 온도는 10도 습도는 30%입니다.");
          break;

        /* GAS CHECK */ 
        case "gas_check": 
          xhr.open('GET',host + 'HGAS');
          $('.intent').text("현재 가스 수치는 100 입니다.");
          break;

        /* DUST CHECK */ 
        case "Dust_check":
          xhr.open('GET',host + 'HDUST');
          $('.intent').text("현재 미세먼지는 30으로 좋음 상태입니다.");
          break;
        
        /* MUSIC ON/OFF */
        case "music_off":
          xhr.open('GET',host + 'HFUNBU');
          $('.intent').text("음악을 재생합니다.");
          break;
        case " music_on":
          xhr.open('GET',host + 'HWARMBU');
          $('.intent').text("음악을 재생합니다.");
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
  