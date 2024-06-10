class WsClient {
    constructor(uri, output) {
        this.uri = uri;
        this.output = output;
        this.reconnect = () => {
            if (this.connected()) {
                return;
            }
            this.socket.close();
            this.connect();
        }
    }

    uri;
    socket;

    output;
    colorMap = ['#AF00FF', '#00FF00', '#FFA100', '#999999']

    connect() {
        let url = this.uri;
        let wsUrlBase;
        if (location.hostname === "localhost" || location.hostname === "127.0.0.1") {
            wsUrlBase = 'ws://esp32mpu6050:80';
        } else {
            wsUrlBase = 'ws://' + location.hostname + ':' + location.port;
        }
        this.socket = new WebSocket(wsUrlBase + url);
        this.socket.onmessage = this.output;
        this.socket.onerror = () => {
            this.setStatusShow();
            setTimeout(this.reconnect, 1000);
        }
        this.socket.onopen = () => {
            console.log("已连接ws");
            this.send("hello");
            this.setStatusShow();
        }

    }

    connected() {
        this.setStatusShow();
        return this.socket && (this.socket.readyState === WebSocket.OPEN || this.socket.readyState === WebSocket.CONNECTING);
    }


    /**
     * @param msg json字符串
     * */
    send(msg) {
        this.socket.send(msg);
    }

    setStatusShow() {
        let wsStatusBar = document.querySelector('#websocket');
        if (!this.socket) {
            wsStatusBar.style.color = this.colorMap[WebSocket.CLOSED];
            return;
        }
        wsStatusBar.style.color = this.colorMap[this.socket.readyState];
    }

}


export default WsClient