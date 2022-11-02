// including essentials libraries starts here
#include <iostream>
#include <string>
#include <fstream>
// including essentials libraries ends here

class Lexer {
    std::ifstream fptr_left;
    std::ifstream fptr_right;
    std::ofstream fptr_output;

    // used to store the position of both file pointers
    int leftptr_pos;
    int rightptr_pos;

    public:
        Lexer () {
            this->fptr_output.open("wiz.lex");

            leftptr_pos = 0;
            rightptr_pos = 0;
        }

        ~Lexer () {
            this->fptr_output.close();
            this->fptr_left.close();
            this->fptr_right.close();
        }

        void move_left_ptr(bool);
        void move_right_ptr();
        char read_left_ptr();
        char read_right_ptr();
        std::string get_token_prefix(std::string);
        void parse(std::string);
        void update(std::string);
};

void Lexer::move_left_ptr(bool is_toright = false) {
    if (is_toright) {
        this->fptr_left.seekg(this->rightptr_pos);
        this->leftptr_pos = this->rightptr_pos;
    } else {
        this->leftptr_pos++;
        this->fptr_left.seekg(this->leftptr_pos);
    }
}

char Lexer::read_left_ptr() {
    char c = this->fptr_left.get();
    this->fptr_left.seekg(this->leftptr_pos);
    return c;
}

void Lexer::move_right_ptr() {
    this->rightptr_pos++;
    this->fptr_right.seekg(this->rightptr_pos);
}

char Lexer::read_right_ptr() {
    char c = this->fptr_right.get();
    this->fptr_right.seekg(this->rightptr_pos);
    return c;
}

std::string get_token_prefix(std::string lexeme) {
    if (TOKEN.is_keyword(lexeme)) return "KY";
    return "ID";
    //if (TOKEN.)
}

void Lexer::update(std::string line) {
    while (TOKEN.is_whitespace(line.back())) line.pop_back();
    this->fptr_output<<line<<'\n'; 
}

void Lexer::parse(std::string infile) {
    // setting up the file pointers
    this->fptr_left.open(infile);
    this->fptr_right.open(infile);

    std::string lexeme;         // stores the token it reads
    std::string stmt;           // stores the parsed statement

    bool is_comment_start = false;

    while (true) {
        // if illegal character is encountered: ignore the character and move on show error at the end
        if (!TOKEN.is_character_set(this->read_right_ptr()) && this->read_right_ptr() != EOF) {
            std::cout<<"Illegal character encountered: "<<this->read_right_ptr()<<std::endl;
            this->move_right_ptr();
            continue;
        }

        // handling comments
        if (this->read_right_ptr() == '#') is_comment_start = true;
        
        while (is_comment_start) {
            this->move_right_ptr();
            
            if (this->read_right_ptr() == '\n' || this->read_right_ptr() == EOF) {
                is_comment_start = false;
                
                if (!lexeme.empty()) {
                    stmt+=lexeme;
                    stmt+=' ';
                    lexeme.clear();
                }
                
                if (!stmt.empty()) {
                    this->update(stmt);
                    stmt.clear();
                }    
            }
        }

        // if whitespace encountered by left pointer
        if (this->read_left_ptr() == ' ') {
            this->move_left_ptr();
            this->move_right_ptr();
            continue;
        }

        // handling EOF and empty lexeme situation
        if (this->read_right_ptr() == EOF && lexeme.empty()) break;

        if ((this->fptr_right.peek() == EOF || this->read_right_ptr() == ' ' || this->read_right_ptr() == '\n') && !lexeme.empty() && lexeme.back() != '\\') { 
            stmt += lexeme;
            stmt += " ";
            
            if(this->read_right_ptr() == '\n' || this->fptr_right.peek() == EOF) {
                this->update(stmt);
                stmt.clear();
            }

            lexeme.clear();
            
            if (this->fptr_right.peek() == EOF) break; else this->move_left_ptr(true);
            continue;
        }

        if (this->read_right_ptr() == '\n') {
            // if escape character \ occurs before \n do not treat \n as a newline
            if (lexeme.back() == '\\') lexeme.pop_back();
        } else {
            if (this->read_right_ptr() != EOF) lexeme.push_back(this->read_right_ptr());
        }
        if (this->read_right_ptr() != EOF) this->move_right_ptr();
    }
}

Lexer LEXER;