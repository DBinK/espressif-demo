import { createApp } from 'vue'
import App from './App.vue'
import {init} from './js/init'
import '@arco-design/web-vue/dist/arco.css';
init()
createApp(App).mount('#app')
