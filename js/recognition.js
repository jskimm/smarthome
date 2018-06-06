/* SPEECH RECOGNITION SETUP */
var recognition = new webkitSpeechRecognition();
recognition.continuous = false;
// recognition.interimResults = true;
console.log(recognition);
recognition.lang = "ko-KR";
/* COMPONENT */
recognition.onresult = function(event) {
  var sentence = event.results[event.results.length-1][0].transcript.trim();
  var url = 'https://westus.api.cognitive.microsoft.com/luis/v2.0/apps/32f607a4-d5db-420d-8676-2d97d034dd92?subscription-key=8eb8c0a2793f41bea76e8fa347684af7&verbose=true&timezoneOffset=0&q=';
  var host = 'http://192.168.43.142/';
  $('.text').removeClass('fadeInUp');
  console.log(sentence);
  
  /* XHR implementation */
  function xhr_open(method, path)  {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
      if(xhr.readyState==4 && xhr.status==200) {
        return true;
      }
      else return false;
    }
    xhr.open(method, host+path);
    xhr.send();
  }

  function xhr_get_value(method, id, callback) {
    var xhr = new XMLHttpRequest();
    xhr.open(method, host);
    xhr.onreadystatechange = function() {
      if(xhr.readyState==4 && xhr.status==200) {
        function parseHTML(html) {
            var t = document.createElement('template');
            t.innerHTML = html;
            return t.content.cloneNode(true);
        }
        callback(parseHTML(xhr.response).getElementById(id).innerHTML);
      } else return false;
    }
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
        case "All_light_off":
          xhr_open('GET', 'LALL');
          $('.intent').text("모든 불을 껐습니다.");
          break;
 
          case "All_light_on":
          xhr_open('GET', 'HALL');
          $('.intent').text("모든 불을 켰습니다.");
          break;

        case "Red_light_off":
          xhr_open('GET', 'LR');
          $('.intent').text("빨간 불을 껐습니다.");
          break;
        case "Red_light_on":
          xhr_open('GET', 'HR')
          $('.intent').text("빨간 불을 켰습니다.");
          break;

        case "Green_light_off":
          xhr_open('GET', 'LG');
          $('.intent').text("녹색 불을 껐습니다.");
          break;
        case "Green_light_on":
          xhr_open('GET', 'HG');
          $('.intent').text("녹색 불을 켰습니다.");
          break;
        case "Blue_light_off":
          xhr_open('GET', 'LB');
          $('.intent').text("파랑 불을 껐습니다.");
          break;
        case "Blue_light_on":
          xhr_open('GET', 'HB');
          $('.intent').text("파랑 불을 켰습니다.");
          break;
        
          case "livingroom_light_on":
          xhr_open('GET', 'HLIVL');
          $('.intent').text("거실 불을 켰습니다.");
          break;
        case "livingroom_light_off":
          xhr_open('GET', 'LLIVL');
          $('.intent').text("거실 불을 껐습니다.");
          break;  

        /* TEMP HUM CHECK */ 
        case "Temp & humidity":
          xhr_get_value('GET', 'tmp', function(tmp) {
            xhr_get_value('GET', 'hum', function(hum) {
              $('.intent').text("현재 온도는 "+tmp+"도 습도는 "+hum+"%입니다.");
            });
          });
          break;

        /* Air conditioner*/
        case "air_conditioner_off": 
          xhr_open('GET', 'LAIRL');
          $('.intent').text("에어컨 껐습니다.");
          break;

        case "air_conditioner_on": 
          xhr_open('GET', 'HAIRL');
          $('.intent').text("에어컨 켰습니다.");
          break;
        /* WINDOW */ 
        case "window_open": 
          xhr_open('GET', 'HS');
          $('.intent').text("창문이 열립니다.");
          break;

        case "window_close": 
          xhr_open('GET', 'LS');
          $('.intent').text("창문이 닫힙니다.");
          break;

        /* GAS CHECK */ 
        case "Gas_check": 
          xhr_get_value('GET', 'gas', function(gas) {
            if(gas<500){
              $('.intent').text("현재 가스 수치는 안전 입니다.");  
            }else if(gas >=500 && gas<700) {
              $('.intent').text("현재 가스 수치는 보통 입니다.");
            }else{
              $('.intent').text("현재 가스 수치는 위험 입니다.");
            }
          });
          break;

        /* DUST CHECK */ 
        case "Dust_check":
          xhr_get_value('GET', 'dust', function(dust) {
            if(dust<0){
              $('.intent').text("현재 미세먼지 농도는 좋음 입니다.");  
            }else if(dust >=0 && dust<100) {
              $('.intent').text("현재 미세먼지 농도는 보통 입니다.");
            }else{
              $('.intent').text("현재 미세먼지 농도는 나쁨 입니다.");
            }
           
          });
          break;
        
        /* MUSIC ON/OFF */
        case "music_off":
          xhr_open('GET', 'HFUNBU');
          $('.intent').text("음악을 재생합니다.");
          break;
        case " music_on":
          xhr_open('GET', 'HBU');
          $('.intent').text("음악을 재생합니다.");
          break;

        /* SEARCH */
        case "Search":
          res.entities.forEach(function(item) {
            if (item.type == "Search"){
              window.open('https://www.google.com/search?q='+item.entity,'_blank');
            }
          });
          break;

        /* EXCEPTION */ 
        case "Exception":
          $('.intent').text("")
          break;
      }

  
      $('.intent').append("<br/>"+"("+(res.topScoringIntent.score*100).toFixed(2)+"%)")
      if (res.entities.length === 0) {
        $('.text').addClass('fadeInUp');
        $('.text').text(sentence);
      }
      // res.entities.forEach(function(item) {
      //   console.log(item);
      //   var highlight = $('<b/>').text(txt.slice(item.startIndex, item.endIndex+1));
      //   highlight.addClass('highlight');
      //   $('.text').addClass('fadeInUp');
      //   $('.text').html(highlight);
      //   $('.text').append(sentence.slice(item.endIndex+1));
      // })

    }
    
  });
}
recognition.start();
setInterval(function(){recognition.start();}, 6000);
// recognition.start();
  
