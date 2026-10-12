#include "ember.hpp"

// Turn a word into a number. Accepts decimal (65) and hex (0x41).
// Returns false if the word is not a number at all.
static bool parse_number(const string& word, long& out) {
    try {
        size_t used = 0;
        // base 0 means: look at the prefix. "0x41" is hex, "65" is decimal.
        out = stol(word, &used, 0);
        return used == word.size(); // reject things like "12abc"
    } catch (...) {
        return false;
    }
}

//  =============================================================================
//  ==============  COMMANDS
//
void cmd_get(Memory& mem, istringstream& words) {
    string a;
    long addr = 0;
    if (!(words >> a) || !parse_number(a, addr)) {
        cout << "usage: get <addr>\n";
        return;
    }
    if (addr < 0) {
        cout << "address must not be negative\n";
        return;
    }
    
    show_byte(mem_get(mem, static_cast<size_t>(addr)));
}

void cmd_set(Memory& mem, istringstream& words) {
    string a, v;
    long addr = 0, value = 0;
    if (!(words >> a) || !(words >> v) || !parse_number(a, addr) || !parse_number(v, value)) {
        cout << "usage: set <addr> <value>\n";
        return;
    }
    if (addr < 0) {
        cout << "address must not be negative\n";
        return;
    } 
    if (value < 0 || value > 255) {
        // A cell holds ONE byte. 256 does not fit. Lab 1, theory 3.
        cout << "a byte is 0..255, got " << value << '\n';
        return;
    }
    if (!mem_set(mem, static_cast<size_t>(addr), static_cast<Byte>(value))) {
        cout << "address " << addr << " is outside 0.." << MEM_SIZE - 1 << '\n';
        return;
    }
}

void cmd_inc(Memory& mem, istringstream& words) {
    string a;
    long addr = 0;

    if (!(words >> a) || !parse_number(a, addr)) {
        cout << "usage: inc <addr>\n";
    } else if (addr < 0) {
        cout << "address must not be negative\n";
    } else {
        bool result = inc(mem, addr);

        if (!result) {
            cout << "Error during cmd: inc <addr>, addr = " << addr << "\n";
        }
    }
}

void cmd_reg(CPU& cpu, istringstream& words) {
    string r, v;
    long value = 0;
    
    if (
        !(words >> r) || !(words >> v) ||   //  чи достатньо елементів у команді?
        !(r == "a" || r == "b") ||          //  r є "а" або "b"?
        !parse_number(v, value) ||          //  чи v є цілим числом?
        value < 0 || value > 255            //  чи v в межах 0-255
    ) {
        cout << "usage: reg <a|b> <0..255>\n";
        return;
    }
    
    cpu.a = (r == "a") ? (Byte)value : cpu.a;
    cpu.b = (r == "b") ? (Byte)value : cpu.b;
}


void cmd_get16(Memory& mem, istringstream& words){
    string a;
    long addr = 0;
    if (!(words >> a) || !parse_number(a, addr)) {
        cout << "usage: get16 <addr>\n";
        return;
    }
    if (addr < 0) {
        cout << "address must not be negative\n";
        return;
    }
    U16 result = get16(mem, addr);

    show_two_bytes(result);
}


void cmd_set16(Memory& mem, istringstream& words){
    string a, v;
    long addr = 0, value = 0;
    if (!(words >> a) || !(words >> v) || !parse_number(a, addr) || !parse_number(v, value)) {
        cout << "usage: set16 <addr> <value>\n";
        return;
    }
    if (addr < 0) {
        cout << "address must not be negative\n";
        return;
    } 
    if (value < 0 || value > 65535) {
        // A cell holds TWO byte. Value less than 65536 (2^16) 
        cout << "a byte is 0..65535, got " << value << '\n';
        return;
    }

    if(!set16(mem, addr, static_cast<U16>(value))){
        cout << "address " << addr << " is outside 0.." << MEM_SIZE - 1 << '\n';
    }
}




void cmd_run(CPU &cpu){
    while(!cpu.halted){
        step(cpu);
    }
}