<template>
  <div id="appRoot" class="user_info_pc">
    <a-layout style="height: 400px;">
      <a-layout-header>
        <a-space direction="vertical" :size="16" style="display: block;">
          <a-row class="grid-demo">
            <a-col :offset="9" :span="8">
              <div id="titleBar">ESP32 Vue Demo</div>
            </a-col>
          </a-row>
        </a-space>
      </a-layout-header>
      <a-layout-content>
        <a-space direction="vertical" :size="16" style="display: block;">
          <a-row class="grid-demo">
            <a-col :span="4">
            </a-col>
            <a-col :span="6">
              <a-card :style="{ width: '100%' }" title="系统信息">
                <a-space direction="vertical" fill>
                  <a-space>
                    <span class="item_name">开发板类型</span>
                    <span class="item_value">{{ boardId }}</span>
                  </a-space>
                  <a-space>
                    <span class="item_name">CPU频率</span>
                    <span class="item_value">{{ cpuFreq }}</span>
                  </a-space>
                  <a-space>
                    <span class="item_name">CPU核心数</span>
                    <span class="item_value">{{ chipCores }}</span>
                  </a-space>
                  <a-space>
                    <span class="item_name">芯片型号</span>
                    <span class="item_value">{{ chipModel }}</span>
                  </a-space>
                  <a-space>
                    <span class="item_name">版本</span>
                    <span class="item_value">{{ chipRevision }}</span>
                  </a-space>
                  <a-space>
                    <span class="item_name">flash容量</span>
                    <span class="item_value">{{ flashSize }}</span>
                  </a-space>
                  <a-space>
                    <span class="item_name">flash模式</span>
                    <span class="item_value">{{ flashMode }}</span>
                  </a-space>
                  <a-space>
                    <span class="item_name">flash速度</span>
                    <span class="item_value">{{ flashSpeed }}</span>
                  </a-space>
                </a-space>
              </a-card>
            </a-col>
            <a-col offset="1" :span="6">
              <a-card :style="{ width: '100%' }" title="控制">
                <a-switch checked-color="#14C9C9" unchecked-color="#F53F3F" @change="ledSwitch"
                          :model-value="ledStatus"/>
              </a-card>
            </a-col>
          </a-row>
        </a-space>
      </a-layout-content>
      <a-layout-footer>
      </a-layout-footer>
    </a-layout>
  </div>
</template>
<script>
import axios from 'axios';
import {getFlashModeName} from "@/js/meta_define";
import {Message} from "@arco-design/web-vue";

export default {
  data() {
    return {
      boardId: 'unknown',
      cpuFreq: '',
      chipCores: '',
      chipModel: '',
      chipRevision: '',
      flashSize: '',
      flashMode: '',
      flashSpeed: '',
      ledStatus: false
    }
  }, methods: {
    ledSwitch() {
      this.ledStatus = !this.ledStatus;
      axios.get(this.ledStatus ? '/switchLed/0' : '/switchLed/1').then((resp) => {
        Message.info({content: resp.data, showIcon: true});
      }).catch((e)=>{
        console.log(e);
        Message.info({content: "发生异常", showIcon: true});
      })
    }
  }, created() {
    axios.get('/sysInfo').then((resp) => {
      let data = resp.data;
      this.boardId = data.boardId;
      this.cpuFreq = data.cpuFreq + 'MHz';
      this.chipCores = data.chipCores;
      this.chipModel = data.chipModel;
      this.chipRevision = data.chipRevision;
      this.flashSize = data.flashSize;
      this.flashSpeed = data.flashSpeed;
      this.flashMode = getFlashModeName(data.flashMode);
      console.log(data);
    }).catch((err) => {
      console.log(err)
    })
  }
}
</script>
<style>
#appRoot {
  padding-top: 3rem;
  margin-left: 0;
}

#titleBar {
  margin-bottom: 2rem;
  font-size: 3rem;
  color: cadetblue;
}

.user_info_pc {

  color: var(--color-text-2);
  background: var(--color-bg-3);
}

body {
  background: var(--color-bg-3);
}

.item_name {
  width: 5rem;
}
</style>