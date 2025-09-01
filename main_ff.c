#include "first_follow.h"
#include <stdio.h>

int main() {
    // Programa
    add_production('P', "m d B");

    // Bloco
    add_production('B', "i D C f");

    // Declarações
    add_production('D', "L v T D");  
    add_production('D', "e");        

    add_production('L', "d R");      
    add_production('R', ", d R");    
    add_production('R', "e");        

    add_production('T', "t");        
    add_production('T', "h");        
    add_production('T', "v");        

    // Comandos
    add_production('C', "S C");     
    add_production('C', "e");       

    add_production('S', "d E");     
    add_production('S', "k E S x S");
    add_production('S', "w E S");   
    add_production('S', "r S a E"); 
    add_production('S', "B");       

    // Expressões
    add_production('E', "T X");
    add_production('X', "+ T X");
    add_production('X', "- T X");
    add_production('X', "e");

    add_production('T', "F Y");
    add_production('Y', "* F Y");
    add_production('Y', "/ F Y");
    add_production('Y', "e");

    add_production('F', "( E )");
    add_production('F', "d");
    add_production('F', "n");
    add_production('F', "h"); // char literal

    compute_first_follow();
    print_first_follow();
    return 0;
}
