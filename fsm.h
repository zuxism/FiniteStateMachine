#ifndef NUM_STATES
    #define NUM_STATES 5
#endif

#define FSM_STATE(state_name) const char* state_name = #state_name

typedef struct {
   const char* state_name   = nullptr;
   void (*on_enter)() = nullptr;
   void (*loop)    () = nullptr;
   void (*on_exit) () = nullptr;
} state_info_t;

state_info_t states[NUM_STATES];
state_info_t* active_state_info = nullptr;

state_info_t* serach_state_name(const char* this_state_name) {
    for (int i=0; i<NUM_STATES; i++) {
        if (states[i].state_name == this_state_name)
            return &states[i];
    }
    return nullptr;
}

state_info_t* add_state_name(const char* this_state_name) {
    for (int i=0; i<NUM_STATES; i++) {
        if (states[i].state_name == nullptr) {
            states[i].state_name = this_state_name;
            return &states[i];
        }
    }
    return nullptr;
}

state_info_t* search_or_add_state_name(const char* this_state_name) {
    state_info_t* this_state_info = nullptr;
    
    this_state_info = serach_state_name(this_state_name);
    
    // if state not found try to add it to next available spot in list
    if (this_state_info == nullptr) this_state_info = add_state_name(this_state_name);
    
    return this_state_info;
}



void fsm_on_enter(const char* this_state_name, void (*this_function)()) {
    
    state_info_t* this_state_info = search_or_add_state_name(this_state_name);
    
    // if nullptr no space left
    if (this_state_info == nullptr) {
        Serial.println("Not adding anything! Number of states reached NUM_STATES");
        return;
    }
    
    this_state_info->on_enter = this_function;
}

void fsm_loop(const char* this_state_name, void (*this_function)()) {
    
    state_info_t* this_state_info = search_or_add_state_name(this_state_name);
    
    // if nullptr no space left
    if (this_state_info == nullptr) {
        Serial.println("Not adding anything! Number of states reached NUM_STATES");
        return;
    }
    
    this_state_info->loop = this_function;
}

void fsm_on_exit(const char* this_state_name, void (*this_function)()) {
    
    state_info_t* this_state_info = search_or_add_state_name(this_state_name);
    
    // if nullptr no space left
    if (this_state_info == nullptr) {
        Serial.println("Not adding anything! Number of states reached NUM_STATES");
        return;
    }
    
    this_state_info->on_exit = this_function;
}




#ifndef NUM_TRANZITIONS
    #define NUM_TRANZITIONS 10
#endif

typedef struct {
   const char* from_state_name = nullptr;
   const char* to_state_name   = nullptr;
   void (*function)()    = nullptr;
} tranzition_info_t;

tranzition_info_t tranzitions[NUM_TRANZITIONS];

tranzition_info_t* search_tranzition_names(const char* from_this_state_name, const char* to_this_state_name) {
    for (int i=0; i<NUM_TRANZITIONS; i++) {
        if (tranzitions[i].from_state_name == from_this_state_name && tranzitions[i].to_state_name == to_this_state_name)
            return &tranzitions[i];
    }
    return nullptr;
}

tranzition_info_t* add_tranzition_names(const char* from_this_state_name, const char* to_this_state_name) {
    for (int i=0; i<NUM_TRANZITIONS; i++) {
        if (tranzitions[i].from_state_name == nullptr && tranzitions[i].to_state_name == nullptr) {
            tranzitions[i].from_state_name = from_this_state_name;
            tranzitions[i].to_state_name   = to_this_state_name;
            return &tranzitions[i];
        }
    }
    return nullptr;
}

tranzition_info_t* search_or_add_tranzition_names(const char* from_this_state_name, const char* to_this_state_name) {
    tranzition_info_t* this_tranzition_info = nullptr;
    
    this_tranzition_info = search_tranzition_names(from_this_state_name, to_this_state_name);
    
    // if tranzition not found try to add it to next available spot in list
    if (this_tranzition_info == nullptr) this_tranzition_info = add_tranzition_names(from_this_state_name, to_this_state_name);
    
    return this_tranzition_info;
}

void fsm_set_tranzition(const char* from_this_state_name, const char* to_this_state_name, void (*this_function)()) {
    
    tranzition_info_t* this_tranzition_info = search_or_add_tranzition_names(from_this_state_name, to_this_state_name);
    
    // if nullptr no space left
    if (this_tranzition_info == nullptr) {
        Serial.println("Not adding anything! Number of tranzitions reached NUM_STATES");
        return;
    }
    
    this_tranzition_info->function = this_function;
    
}



const char* fsm_active_state() {
    return active_state_info->state_name;
}



void _fsm_enter_state(const char* new_state_name) {
    state_info_t* old_state_info = active_state_info;
    state_info_t* new_state_info = serach_state_name(new_state_name);
    tranzition_info_t* tranzition_info = nullptr;
    
    if (old_state_info != nullptr) {
        if (old_state_info->on_exit != nullptr)
            old_state_info->on_exit();
        tranzition_info = search_tranzition_names(old_state_info->state_name, new_state_name);
    }
    
    if (tranzition_info != nullptr) {
        if (tranzition_info->function != nullptr)
            tranzition_info->function();
    }
    
    if (new_state_info != nullptr) {
        if (new_state_info->on_enter != nullptr)
            new_state_info->on_enter();
        active_state_info = new_state_info;
    }
}

#define fsm_enter_state(state) {_fsm_enter_state(state); return;}


void fsm_loop() {
    if (active_state_info != nullptr) {
        if (active_state_info->loop != nullptr)
            active_state_info->loop();
    }    
}














