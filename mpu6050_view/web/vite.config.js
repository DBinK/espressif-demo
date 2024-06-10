import { defineConfig } from 'vite'

import viteCompression from 'vite-plugin-compression'

// https://vitejs.dev/config/
export default defineConfig({
    plugins: [viteCompression({
        filter: /\.(js|css|json|txt|html|ico|svg|obj)(\?.*)?$/i, // 需要压缩的文件
        threshold: 1, // 文件容量大于这个值进行压缩
        algorithm: 'gzip', // 压缩方式
        ext: 'gz', // 后缀名
        deleteOriginFile: true, // 压缩后是否删除压缩源文件
    })
    ]
})