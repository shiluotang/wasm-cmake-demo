# README

## coc.nvim support 

+ add current content in coc local settings(`$project_root/.vim/coc-settings.json`)

```json
{
    "clangd.arguments": [
        "--query-driver=C:\\Users\\xxx\\emsdk\\upstream\\emscripten\\em*"
    ]
}
```
**ATTENTION for windows, DO NOT add ".bat" suffix**

+ There's no need for ".clangd" file under project root
