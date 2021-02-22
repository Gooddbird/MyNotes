#include<iostream>


int make_num = 0;
int left_copy_num = 0;
int right_copy_num = 0;
class MyString{

public:
    MyString(const char* s = 0) {
        
        if(s) {
            m_data = new char[strlen(s + 1)];
            strcpy(m_data, s);
        } else {
            m_data = new char[1];
            *m_data = '\0';
        }
    }

    // 拷贝构造函数
    MyString(const MyString& s) {
        left_copy_num++;
        m_data = new char[strlen(s.m_data) + 1];
        strcpy(m_data, s.m_data);
    }

    MyString& operator=(const MyString& s) {
        if(this == &s) {
            return *this;
        }
        delete[] m_data;
        m_data = new char[strlen(s.m_data) + 1];
        strcpy(m_data, s.m_data);
        return *this;
    }

    ~MyString() {
        if(m_data) {
            delete[] m_data;
            m_data = nullptr;
        }
    }

private:
    char* m_data;
};
