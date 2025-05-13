# todo-terminal
Todo list but in terminal

# todo-stream

## .todo file format
 
    TODO // header
    0xCCCCCCCCCCCCCCCC //header 8bytes padding
    
    size_t n
    
    size_t titleSize[];
    char *title[];
    
    size_t descSize[];
    char *desc[];
        
    size_t priority[];
    time_t created[];
    time_t deadline[];
    
    0xCCCCCCCCCCCCCCCC //8bytes pading
