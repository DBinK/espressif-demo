import * as THREE from 'three';

import {OBJLoader} from 'three/addons/loaders/OBJLoader.js';
import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
import WsClient from "./websocket.js";

let camera, scene, renderer;
let object;
let cmdChannel;
init();

function initWs() {
    cmdChannel = new WsClient('/ws', (eve) => {
        let data = eve.data;
        if (!data) {
            return;
        }
        if (data === "hello") {
            console.log("ack");
            return;
        }
        try {
            let array = JSON.parse(data);
            if (!(array instanceof Array)) {
                return;
            }
            object.rotation.set(array[0] / 180, array[1] / 180, array[2] / 180);
            renderer.render(scene, camera);
            // console.log(`esp32:\x1b[32m${data}\x1b[0m`);
        } catch (e) {
            console.error(e);
        }

    });
    cmdChannel.connect();
}

function init() {

    camera = new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.1, 2000);
    camera.position.z = 1.5;
    camera.position.x = 1.5;
    camera.position.y = 1.5;
    // scene

    scene = new THREE.Scene();

    const ambientLight = new THREE.AmbientLight(0xffffff);
    scene.add(ambientLight);

    const pointLight = new THREE.PointLight(0xffffff, 15);
    camera.add(pointLight);
    scene.add(camera);

    // manager

    function loadModel() {
        object.scale.setScalar(0.01);
        scene.add(object);

        render();

    }

    const manager = new THREE.LoadingManager(loadModel);

    const axesHelper = new THREE.AxesHelper(10);
    scene.add(axesHelper);

    // model

    function onProgress(xhr) {

        if (xhr.lengthComputable) {

            const percentComplete = xhr.loaded / xhr.total * 100;
            console.log('model ' + percentComplete.toFixed(2) + '% downloaded');

        }

    }

    function onError() {
    }

    const loader = new OBJLoader(manager);
    loader.load('/model/plane.obj', function (obj) {
        object = obj;
    }, onProgress, onError);

    //

    renderer = new THREE.WebGLRenderer({antialias: true});
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.setSize(window.innerWidth, window.innerHeight);
    document.body.appendChild(renderer.domElement);

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.minDistance = 2;
    controls.maxDistance = 5;
    controls.addEventListener('change', render);

    window.addEventListener('resize', onWindowResize);
    initWs();
    setInterval(() => {
        if (!cmdChannel.connected()) {
            cmdChannel.reconnect();
        }
    }, 3000);
}

function onWindowResize() {

    camera.aspect = window.innerWidth/ window.innerHeight;
    camera.updateProjectionMatrix();

    renderer.setSize(window.innerWidth, window.innerHeight);

}

function render() {

    renderer.render(scene, camera);

}
