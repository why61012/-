#ifndef LIBRARY_MANAGEMENT_SYSTEM_H
#define LIBRARY_MANAGEMENT_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <ctime>
#include <algorithm>

// 图书信息结构体
struct Book {
    int id;
    std::string title;
    std::string author;
    std::string isbn;
    bool is_borrowed;
    std::string borrower;
    time_t borrow_date;
    
    Book() : id(0), is_borrowed(false), borrow_date(0) {}
    
    Book(int id, const std::string& title, const std::string& author, 
         const std::string& isbn)
        : id(id), title(title), author(author), isbn(isbn), 
          is_borrowed(false), borrow_date(0) {}
};

// 图书馆管理类
class LibraryManager {
private:
    std::map<int, Book> books;
    int next_book_id;
    
public:
    LibraryManager() : next_book_id(1) {
        // 初始化示例数据
        initSampleData();
    }
    
    // 初始化示例数据
    void initSampleData() {
        addBook("The Great Gatsby", "F. Scott Fitzgerald", "978-0-7432-7356-5");
        addBook("To Kill a Mockingbird", "Harper Lee", "978-0-06-112008-4");
        addBook("1984", "George Orwell", "978-0-451-52494-2");
        addBook("Pride and Prejudice", "Jane Austen", "978-0-14-143951-8");
        addBook("The Catcher in the Rye", "J.D. Salinger", "978-0-316-76948-0");
    }
    
    // 添加新图书
    int addBook(const std::string& title, const std::string& author, 
                const std::string& isbn) {
        Book book(next_book_id, title, author, isbn);
        books[next_book_id] = book;
        return next_book_id++;
    }
    
    // 获取所有图书
    std::vector<Book> getAllBooks() const {
        std::vector<Book> result;
        for (const auto& pair : books) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    // 根据ID获取图书
    Book* getBook(int id) {
        auto it = books.find(id);
        if (it != books.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    // 更新图书
    bool updateBook(int id, const std::string& title, const std::string& author,
                   const std::string& isbn) {
        auto it = books.find(id);
        if (it != books.end()) {
            it->second.title = title;
            it->second.author = author;
            it->second.isbn = isbn;
            return true;
        }
        return false;
    }
    
    // 删除图书
    bool deleteBook(int id) {
        return books.erase(id) > 0;
    }
    
    // 借阅图书
    bool borrowBook(int id, const std::string& borrower) {
        auto it = books.find(id);
        if (it != books.end() && !it->second.is_borrowed) {
            it->second.is_borrowed = true;
            it->second.borrower = borrower;
            it->second.borrow_date = time(nullptr);
            return true;
        }
        return false;
    }
    
    // 归还图书
    bool returnBook(int id) {
        auto it = books.find(id);
        if (it != books.end() && it->second.is_borrowed) {
            it->second.is_borrowed = false;
            it->second.borrower = "";
            it->second.borrow_date = 0;
            return true;
        }
        return false;
    }
    
    // 搜索图书
    std::vector<Book> searchBooks(const std::string& keyword) const {
        std::vector<Book> result;
        std::string lower_keyword = keyword;
        std::transform(lower_keyword.begin(), lower_keyword.end(), 
                      lower_keyword.begin(), ::tolower);
        
        for (const auto& pair : books) {
            std::string title = pair.second.title;
            std::string author = pair.second.author;
            std::transform(title.begin(), title.end(), title.begin(), ::tolower);
            std::transform(author.begin(), author.end(), author.begin(), ::tolower);
            
            if (title.find(lower_keyword) != std::string::npos ||
                author.find(lower_keyword) != std::string::npos) {
                result.push_back(pair.second);
            }
        }
        return result;
    }
};

#endif // LIBRARY_MANAGEMENT_SYSTEM_H
