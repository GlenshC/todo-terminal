# todo-terminal
Todo list but in terminal

# todo-stream

## .todo file format
 
    "TODO" // 4-bytes header and pad with 4 bytes for 8 byte alignment
    "1.0-beta" // 16-bytes version
    
    0xCCCCCCCCCCCCCCCC //header 8bytes padding
    
    size_t n // 8 bytes
    
    size_t titleSize[]; // 8 bytes
    char *title[]; // not 8 bytes aligned
    
    size_t descSize[];
    char *desc[];
        
    size_t priority[];
    time_t created[];
    time_t deadline[];
    
    0xCCCCCCCCCCCCCCCC //8bytes pading
