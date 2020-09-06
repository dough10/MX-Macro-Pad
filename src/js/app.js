(function () {
 /**
  *
  */
  function qs(selector, scope) {
    return (scope || document).querySelector(selector)
  }

 /**
  *
  */
  function qsa(selector, scope) {
    return (scope || document).querySelectorAll(selector)
  }

 /**
  *
  */
  function loadCSSFile(src) {
    return new Promise((resolve, reject) => {
      const css = document.createElement( "link" );
      css.href = src;
      css.type = "text/css";
      css.rel = "stylesheet";
      css.onload = resolve;
      css.onerror = reject;
      qs('head').appendChild(css);
    });
  }

 /**
  *
  */
  function loadJSFile(src) {
    return new Promise((resolve, reject) => {
      const script = document.createElement('script');
      script.async = true;
      script.src = src;
      script.onload = resolve;
      script.onerror = reject;
      qs('body').appendChild(script);
    });
  }


 /**
  * determine whice transition event is being used
  */
  function whichTransitionEvent() {
    let t;
    const el = document.createElement('fakeelement');
    const transitions = {
      'WebkitTransition' :'webkitTransitionEnd',
      'MozTransition'    :'transitionend',
      'MSTransition'     :'msTransitionEnd',
      'OTransition'      :'oTransitionEnd',
      'transition'       :'transitionEnd'
    };
    for (t in transitions) {
      if (el.style[t] !== undefined ) {
        return transitions[t];
      }
    }
  }
  // the browser transition event name
  const transitionEvent = whichTransitionEvent();

 /**
  * animate transform / opacity on a give element
  *
  * @param {HTMLElement} el *required*
  * @param {String} transtorm *required*
  * @param {Number} time
  * @param {Number} opacity
  * @param {Number} delay
  */
  function animateElement(el, transform, time, opacity, delay) {
    return new Promise(resolve => {
      if (el.style.transform === transform) {
        resolve();
        return;
      }
      const animationEnd = _ => {
        el.removeEventListener(transitionEvent, animationEnd);
        el.style.willChange = 'initial';
        el.style.transition = 'initial';
        requestAnimationFrame(resolve);
      };
      if (!time) {
        time = 300;
      }
      if (!delay) {
        delay = 0;
      }
      el.addEventListener(transitionEvent, animationEnd, true);
      el.style.willChange = 'transform opacity';
      el.style.transition = `all ${time}ms ease-in-out ${delay}ms`;
      requestAnimationFrame(_ => {
        el.style.transform = transform;
        if (opacity !== undefined) {
          el.style.opacity = opacity;
        }
      });
    });
  }

 /**
  * fade out opacity of a given element
  *
  * @param {HTMLElement} el
  */
  function fadeOut(el) {
    return new Promise(resolve => {
      if (el.style.opacity === 0) {
        resolve();
        return;
      }
      let timer = 0;
      var animationEnd = _ => {
        clearTimeout(timer);
        el.removeEventListener(transitionEvent, animationEnd);
        el.style.willChange = 'initial';
        el.style.transition = 'initial';
        requestAnimationFrame(resolve);
      };
      el.addEventListener(transitionEvent, animationEnd, true);
      el.style.willChange = 'opacity';
      el.style.transition = 'opacity 200ms ease-in-out 0s';
      requestAnimationFrame(_ => {
        timer = setTimeout(animationEnd, 250);
        el.style.opacity = 0;
      });
    });
  }

 /**
  * fade in opacity of a given element
  *
  * @param {HTMLElement} el
  */
  function fadeIn(el) {
    return new Promise(resolve => {
      if (el.style.opacity === 1) {
        resolve();
        return;
      }
      let timer = 0;
      const animationEnd = _ => {
        clearTimeout(timer);
        el.removeEventListener(transitionEvent, animationEnd);
        el.style.willChange = 'initial';
        el.style.transition = 'initial';
        requestAnimationFrame(resolve);
      };
      el.addEventListener(transitionEvent, animationEnd, true);
      el.style.willChange = 'opacity';
      el.style.transition = 'opacity 200ms ease-in-out 0s';
      requestAnimationFrame(_ => {
        timer = setTimeout(animationEnd, 250);
        el.style.opacity = 1;
      });

    });
  }

 /**
  * loads files needed for paper ripple effect
  */
  function loadRipples() {
    return new Promise((resolve, reject) => {
      loadCSSFile("css/paper-ripple.min.css")
      .then(_ => loadJSFile('js/paper-ripple.min.js')
      .then(_ => setTimeout(_ => applyRipples(resolve), 50)).catch(reject));
    });
  }

 /**
  * apply ripple effect to element
  */
  function attachButtonRipple(button) {
    button.PaperRipple = new PaperRipple();
    button.appendChild(button.PaperRipple.$);
    button.addEventListener('mousedown', ev => button.PaperRipple.downAction(ev));
    button.addEventListener('mouseup', e => button.PaperRipple.upAction());
  }

 /**
  * apply ripple effect to round button
  */
  function attachRoundButtonRipple(button) {
    button.PaperRipple = new PaperRipple();
    button.appendChild(button.PaperRipple.$);
    button.PaperRipple.$.classList.add('paper-ripple--round');
    button.PaperRipple.recenters = true;
    button.PaperRipple.center = true;
    button.addEventListener('mousedown', ev => button.PaperRipple.downAction(ev));
    button.addEventListener('mouseup', ev => button.PaperRipple.upAction());
  }

 /**
  * apply material design ripples to clickable elements
  *
  * @param {Function} resolve - function to be called after all work has been done
  */
  function applyRipples(resolve) {
    qsa('.button').forEach(attachButtonRipple);
    qsa('.circle').forEach(attachButtonRipple);
    qsa('.tab').forEach(attachButtonRipple);
    qsa('.icon-button').forEach(attachRoundButtonRipple);
    qsa('.box').forEach(attachButtonRipple);
    resolve();
  }

 /**
  * display a toast message
  *
  * @param {String} message
  * @param {Number} timeout in seconds  || defualt 5 seconds  ** optional
  */
  class Toast {
    constructor(message, _timeout) {
      // bind this to internal functions
      this._transitionEnd = this._transitionEnd.bind(this);
      this._cleanUp = this._cleanUp.bind(this);
      // create the toast
      this._checkPrevious().then(_ => {
        this._timer = false;
        this._timeout = _timeout * 1000 || 4500;
        this.toast = this._createToast();
        this.toast.addEventListener(transitionEvent, this._transitionEnd, true);
        this.toast.addEventListener('click', this._cleanUp, true);
        this.toast.textContent = message;
        document.querySelector('body').appendChild(this.toast);
        attachButtonRipple(this.toast);
        setTimeout(_ => requestAnimationFrame(_ => {
          this.toast.style.opacity = 1;
          this.toast.style.transform = 'translateY(0px)';
        }), 50);
      });
    }

   /**
    * check if there is a existing toast
    */
    _checkPrevious() {
      return new Promise(resolve => {
        let previous = document.querySelector('#toast');
        if (!previous) {
          resolve();
          return;
        }
        previous.addEventListener(transitionEvent, _ => {
          if (previous.parentNode) {
            previous.parentNode.removeChild(previous);
          }
          resolve();
        });
        requestAnimationFrame(_ => {
          previous.style.opacity = 0;
          previous.style.transform = 'translateY(80px)';
        });
      });
    }

   /**
    * returns a new toast html element
    */
    _createToast() {
      const toast = document.createElement('div');
      toast.id ='toast';
      toast.classList.add('toast');
      toast.style.opacity = 0;
      toast.style.transform = 'translateY(80px)';
      toast.style.willChange = 'transform opacity';
      toast.style.transition = 'all 300ms ease-in-out 0ms';
      return toast;
    }

   /**
    * play closing animation and remove element from document
    */
    _cleanUp() {
      if (this._timer) {
        clearTimeout(this._timer);
        this._timer = false;
      }
      this.toast.addEventListener(transitionEvent, _ => {
        if (this.toast.parentNode) {
          this.toast.parentNode.removeChild(this.toast);
        }
      });
      requestAnimationFrame(_ => {
        this.toast.style.opacity = 0;
        this.toast.style.transform = 'translateY(80px)';
      });
    }

   /**
    * called after opening animation
    * sets up closing animation
    */
    _transitionEnd() {
      this._timer = setTimeout(this._cleanUp, this._timeout);
      this.toast.removeEventListener(transitionEvent, this._transitionEnd);
    }
  }


 /**
  * class used to determine the end of a loop
  *
  * @param {Number} count - length of the loop
  *
  * subtract function that is attached to the returned element should be called once per loop
  */
  class Countdown {
    constructor(count) {
      this.count = count;
      this.el = document.createElement('div');
      this.el.subtract = this._subtract.bind(this);
      return this.el;
    }

   /**
    * counts down to 0 and fires 'zero' event
    */
    _subtract() {
      this.count = this.count - 1;
      if (this.count <= 0) {
        this.el.dispatchEvent(new Event('zero'));
      }
    }
  }

 /**
  * append google fonts top the document
  */
  function loadGoogleFonts() {
    return new Promise(resolve => {
      const cacheDate = 1479259604465;
      const fonts = [
        'https://fonts.googleapis.com/icon?family=Material+Icons'
      ];
      let countdown = new Countdown(fonts.length);
      countdown.atZero(_ => {
        countdown = null;
        requestAnimationFrame(resolve);
        return;
      });
      fonts.forEach(fontURL => {
        const url = attachQueryString(fontURL, `modified=${cacheDate}`);
        loadCSSFile(url).then(countdown.subtract);
      });
    });
  }

 /**
  * attach a query string to a given url
  */
  function attachQueryString(url, query) {
    if (url.hasQueryString()) {
      return `${url}&${query}`;
    }
    return `${url}?${query}`;
  }

 /**
  * determine if a url has a query string already attached
  */
  String.prototype.hasQueryString = function () {
    for (let letter in this) {
      if (this[letter] === '?') {
        return true;
      }
    }
    return false;
  };

 /**
  *
  */
  HTMLElement.prototype.onClick = function (cb) {
    this.addEventListener('click', cb, false);
  };

 /**
  * used in countdown class as a callback when loop is done
  */
  HTMLElement.prototype.atZero = function (cb) {
    this.addEventListener('zero', cb, false);
  };

 /**
  * get josn file of button map data
  */
  function getKeyMap() {
    return new Promise((resolve, reject) => {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", 'keys.json', true);
      xhr.responseType = 'json';
      xhr.onload = resolve;
      xhr.onerror = reject;
      xhr.send();
    });
  }

 /**
  * get josn file of modifiers map data
  */
  function getModifers() {
    return new Promise((resolve, reject) => {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", 'modifiers.json', true);
      xhr.responseType = 'json';
      xhr.onload = resolve;
      xhr.onerror = reject;
      xhr.send();
    });
  }


 /**
  * open button setting panel
  */
  function openSettings(page) {
    return new Promise((resolve, reject) => {
      const settings = qs('#settings');
      const header = qs('#settingHeader');
      const tabs = qs('#tabs');
      const card = qs('#settingsCard');
      if (page === "12") {
        card.style.paddingTop = '0px';
        tabs.style.display = 'block';
        header.textContent = 'Encoder Config';
      } else {
        card.style.removeProperty('padding-top');
        tabs.style.display = 'none';
        header.textContent = 'Button ' + page + ' Config';
      }
      qs('#right').classList.add('active');
      qs('#left').classList.remove('active');
      settings.style.display = 'flex';
      setTimeout(_ => {
        animateElement(settings, 'translateY(0px)', 350).then(_ => {
          resolve();
        }).catch(reject);
      }, 40);
    });
  }

 /**
  * close button setting panel
  */
  function closeSettings() {
    animateElement(settings, 'translateY(102%)', 350).then(_ => {
      settings.style.display = 'none';
      var loader = qs('#settings-loader');
      loader.style.pointerEvents = 'all';
      qs('#modifiers').value = 'No Modifier';
      fadeIn(loader);
    });
  }

 /**
  * hide loading screen on setting page
  */
  function showSettings() {
    var loader = qs('#settings-loader');
    fadeOut(loader).then(_ => {
      loader.style.pointerEvents = 'none';
    });
  }

 /**
  * sends oprt to the backend for connection and waits for data stream
  */
  function selectPort(ports) {
    for (var i = 0; i < ports.length; i++) {
      ipc.send('selectPort', ports[i]);
      ipc.once('portOpen', (event, response) => {
        lastData = 0;
        setTimeout(() => {
          // no data close port.
          if (!lastData) {
            new Toast('Error Connecting');
            setTimeout(_ => {
              ipc.send('conError', 'Timed Out');
            }, 4500)
            return;
          }
          // show app UI
          animateElement(qs('#loader'), 'translateY(-102%)', 350);
        }, 2000);
      });
    }
  }


 /**
  * process data from serial port
  *
  * @param {Object} event - listener event
  * @param {String / Array} data - can be either string or array. cannying the payload from serial com
  */
  var lastData;
  function processData(e, data) {
    //console.log(data)
    lastData = data;
    // port data for connecting the app
    if (typeof data !== 'string' && Array.isArray(data)) {
      selectPort(data);
      return;
    }
    // button data
    if (data[0] === ':') {
      showSettings()
      var str = data.substring(1);
      var strings = str.split(":");
      qs('#keys').value = Number(strings[1]);
      qs('#modifiers').value = Number(strings[0]);
      return;
    }
    // LED_MODE change notification
    if (data[0] === '?') {
      var mode = Number(data.substring(1));
      if (mode === 0) {
        new Toast('LED: Adjustable', 0.8);
      } else if (mode === 1) {
        new Toast('LED: On Click', 0.8);
      } else if (mode === 2) {
        new Toast('LED: Breath', 0.8);
      } else if (mode === 3) {
        new Toast('LED: Knight Rider', 0.8);
      } else {
        new Toast('LED: Off', 0.8);
      }
      return;
    }
    // led Adjustable Brightness UI
    var val = 255 - Number(data);
    qs('#brightness').value = val;
    var precent = Math.round((val / 255) * 100);
    qs('#text').textContent = 'LED Brightness: ' + precent + '%';
  }


 /**
  * create a option element and append to the parent element
  *
  * @param {String} option - name the client will see in UI
  * @param {Number} value - the decimal identifier for the keystroke
  * @param {Element} parent - the element to append the finished option element
  */
  function makeOption(option, value, parent) {
    var el = document.createElement('option');
    el.textContent = option;
    el.value = value;
    parent.appendChild(el);
  }

 /**
  * add leading zeros
  *
  * @param {Number} thing - the number to add check for the need of more 0's
  * @param {Number} length - the required length of the number
  */
  function addZeros(thing, length) {
    if (thing.length !== length) {
      thing = '0' + thing;
      return addZeros(thing, length);
    }
    return thing;
  }


  function setVal() {
    var val = addZeros(qs('#keys').value, 3);
    var mod = qs('#modifiers').value;
    if (mod !== '0') {
      mod = Number(mod) - 127;
      val = '' + mod + val;
    }
    ipc.send('selectButton', '<' + val + '>');
    new Toast('Keybind Set', 0.8);
    var loader = qs('#settings-loader');
    loader.style.pointerEvents = 'all';
    fadeIn(loader);
  }

  function gotKeyMap(e) {
    return new Promise((resolve, reject) => {
      var response = e.target.response;
      for (var key in response) {
        makeOption(key, response[key], qs('#keys'));
      }
      resolve();
    });
  }

  function gotModifiers(e) {
    return new Promise((resolve, reject) => {
      var response = e.target.response;
      for (var key in response) {
        makeOption(key, response[key], qs('#modifiers'));
      }
      resolve();
    });

  }

  function circleClick(e) {
    var num = e.target.id.substring(1);
    ipc.send('selectButton', '<' + num + '>');
    openSettings(num);
  }

  // electron communication
  var ipc = require('electron').ipcRenderer;


  function setupListeners() {
    // data from electron backend
    ipc.on('data', processData);
    qs('#closeSettings').onClick(closeSettings);
    qs('#setVal').onClick(setVal);
    qs('.circle').onClick(circleClick);
    qsa('.box').forEach(box => box.onClick(_ => {
      // box.id[1] is the numbers @ the end of the clicked element ID
      var num = box.id[1];
      ipc.send('selectButton', '<' + num + '>');
      openSettings(num);
    }));
    qsa('.tab').forEach(tab => tab.onClick(_ => {
      if (tab.classList.contains('active')) {
        return;
      }
      if (tab.id === 'right') {
        qs('#left').classList.remove('active');
        ipc.send('selectButton', '<12>');
      } else {
        qs('#right').classList.remove('active');
        ipc.send('selectButton', '<13>');
      }
      tab.classList.add('active');
      var loader = qs('#settings-loader');
      loader.style.pointerEvents = 'all';
      fadeIn(loader);
    }));
  }


 /**
  *  RUN IT
  */
  window.onload = loadRipples().then(loadGoogleFonts).then(getKeyMap).then(gotKeyMap).then(getModifers).then(gotModifiers).then(setupListeners);
}());
