
" 编译运行
nnoremap <F5> :w<CR> :call CompileRun()<CR>
func! CompileRun()
    exec "make run"
endfunc

