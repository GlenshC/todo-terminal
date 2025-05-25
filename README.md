TODO-TERMINAL
====

Todo list but in terminal

### WHY?

    Because. 
    why use a gui when you can do it in the terminal.
    Gotta play the part you know.

### BUILDING

```
    git clone https://github.com/GlenshC/todo-terminal.git 
    cd todo-terminal
    make
    .build/todo help
```

### USAGE

```
For more information on a specific command, type:
  todo help [COMMAND]

Usage:
  todo [COMMAND] [ARGS]

Commands:
  add        Adds a new todo item.
  clear      Empties the todo list.
  done       Marks a todo as completed.
  edit       Edits a todo item.
  get        Gets the most important task.
  help       Shows help information.
  list       Shows all todos.
  random     Selects a task randomly.
  rm         Removes a todo item.
  undo       Marks a todo as not completed.
  version    Displays the current version.
  view       Views a specific todo item.
```

### FEATURES
 - todo list can be sorted with --sort <type>
 - todo get and random suggest you what to do if you don't know what to do
 - todo get selects the best thing todo base on users energy, deadline, and priority
 - todo random selects a random yet reasonable task
 - todo list is actually a sorted index arr. custom quicksort and binary search.
 - fast af (for my experience so far)
 - writing binary data file in SOA
 - overly engineered sorting and bitwise data handling. (see todolist.priority)
 - might not run on linux (for now, not sure)

### TODO FILE FORMAT ```v1.0b```

8 bytes alignment (for recovery)

    "TODO"     // 4-bytes header and pad with 4 bytes
    "1.0b" // 16-bytes version
    
    0xABCDEFCCCCFEDCBA   // header 8bytes padding
    
    size_t n;
    uint32_t created[];  // stored in minutes
    uint32_t deadline[]; // stored in days

    uint8_t titleSize[]; 
    char *title[];
    
    uint8_t descSize[];
    char *desc[];
    
    uint8_t priority[]; // stored in 2bits
    uint8_t done[];     // stored in 1 bit
    
    0xCCCCCCCCCCCCCCCC //8bytes pading
