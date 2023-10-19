const audioContext = new AudioContext();

function transmit(str) {

    const oscillator = audioContext.createOscillator();
    oscillator.type = "square";
    oscillator.frequency.value = 500;
    oscillator.connect(audioContext.destination)
    oscillator.start();

    const sendlow = () => {
        oscillator.frequency.value = 1000;
    }

    const sendhigh = () => {
        oscillator.frequency.value = 2000;
    }


    let arr = str.toString().split('');
    let binary = '1'; "".padStart(64,"0") + "".padStart(64, "1")

    let ptr = 0;
    let interval

    setTimeout(() => {
        interval = setInterval(() => {
            if (binary[ptr] == '1') {
                sendhigh();
            } else {
                sendlow();
            }

            ptr++;
            if (ptr > binary.length) {
                clearInterval(interval);
                oscillator.frequency.value = 500;
                setTimeout(() =>oscillator.stop(), 500)
                
            }
        }, 1);

    }, 500);


    return interval;
}
transmit("elton");