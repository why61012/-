#include "library.h"
#include <iostream>
#include <sstream>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #define closesocket close
    #define INVALID_SOCKET -1
#endif

// JSON 生成辅助函数
std::string bookToJson(const Book& book) {
    std::stringstream ss;
    ss << "{"
       << "\"id\":" << book.id << ","
       << "\"title\":\"" << book.title << "\","
       << "\"author\":\"" << book.author << "\","
       << "\"isbn\":\"" << book.isbn << "\","
       << "\"is_borrowed\":" << (book.is_borrowed ? "true" : "false") << ","
       << "\"borrower\":\"" << book.borrower << "\""
       << "}";
    return ss.str();
}

std::string booksToJson(const std::vector<Book>& books) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < books.size(); ++i) {
        ss << bookToJson(books[i]);
        if (i < books.size() - 1) ss << ",";
    }
    ss << "]";
    return ss.str();
}

// HTTP 响应生成
std::string createHttpResponse(const std::string& json_body, int status_code = 200) {
    std::string status_text = (status_code == 200) ? "OK" : "Bad Request";
    std::stringstream ss;
    ss << "HTTP/1.1 " << status_code << " " << status_text << "\r\n"
       << "Content-Type: application/json\r\n"
       << "Access-Control-Allow-Origin: *\r\n"
       << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
       << "Access-Control-Allow-Headers: Content-Type\r\n"
       << "Content-Length: " << json_body.length() << "\r\n"
       << "Connection: close\r\n"
       << "\r\n"
       << json_body;
    return ss.str();
}

// 解析请求体中的 JSON
void parseBookJson(const std::string& json_body, std::string& title, 
                   std::string& author, std::string& isbn) {
    // 简单的 JSON 解析
    size_t title_start = json_body.find("\"title\":\"") + 9;
    size_t title_end = json_body.find("\"", title_start);
    title = json_body.substr(title_start, title_end - title_start);
    
    size_t author_start = json_body.find("\"author\":\"") + 10;
    size_t author_end = json_body.find("\"", author_start);
    author = json_body.substr(author_start, author_end - author_start);
    
    size_t isbn_start = json_body.find("\"isbn\":\"") + 8;
    size_t isbn_end = json_body.find("\"", isbn_start);
    isbn = json_body.substr(isbn_start, isbn_end - isbn_start);
}

int main() {
    LibraryManager manager;
    
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup 失败\n";
        return 1;
    }
#endif
    
    // 创建服务器套接字
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "创建套接字失败\n";
        return 1;
    }
    
    // 设置套接字选项，允许地址重用
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    
    // 绑定
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "绑定失败\n";
        closesocket(server_socket);
        return 1;
    }
    
    // 监听
    if (listen(server_socket, 5) == -1) {
        std::cerr << "监听失败\n";
        closesocket(server_socket);
        return 1;
    }
    
    std::cout << "📚 图书馆管理系统服务器启动" << std::endl;
    std::cout << "监听端口: http://localhost:8080" << std::endl;
    std::cout << "按 Ctrl+C 停止服务器\n\n";
    
    // 接受连接循环
    while (true) {
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        
        SOCKET client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) continue;
        
        char buffer[4096] = {0};
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::string request(buffer);
            std::string response;
            
            // 解析 HTTP 请求
            std::istringstream iss(request);
            std::string method, path, http_version;
            iss >> method >> path >> http_version;
            
            std::cout << "请求: " << method << " " << path << std::endl;
            
            // 处理 OPTIONS 请求（CORS）
            if (method == "OPTIONS") {
                response = "HTTP/1.1 200 OK\r\n"
                          "Access-Control-Allow-Origin: *\r\n"
                          "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                          "Access-Control-Allow-Headers: Content-Type\r\n"
                          "Content-Length: 0\r\n"
                          "Connection: close\r\n\r\n";
            }
            // GET /api/books - 获取所有图书
            else if (method == "GET" && path == "/api/books") {
                auto books = manager.getAllBooks();
                response = createHttpResponse(booksToJson(books));
            }
            // GET /api/books/search?q=keyword - 搜索图书
            else if (method == "GET" && path.find("/api/books/search?q=") == 0) {
                std::string keyword = path.substr(20);
                auto books = manager.searchBooks(keyword);
                response = createHttpResponse(booksToJson(books));
            }
            // POST /api/books - 添加新图书
            else if (method == "POST" && path == "/api/books") {
                // 提取请求体
                size_t body_start = request.find("\r\n\r\n");
                if (body_start != std::string::npos) {
                    std::string json_body = request.substr(body_start + 4);
                    std::string title, author, isbn;
                    parseBookJson(json_body, title, author, isbn);
                    
                    int new_id = manager.addBook(title, author, isbn);
                    Book* new_book = manager.getBook(new_id);
                    response = createHttpResponse(bookToJson(*new_book));
                }
            }
            // PUT /api/books/:id - 更新图书
            else if (method == "PUT" && path.find("/api/books/") == 0) {
                int id = std::stoi(path.substr(11));
                size_t body_start = request.find("\r\n\r\n");
                if (body_start != std::string::npos) {
                    std::string json_body = request.substr(body_start + 4);
                    std::string title, author, isbn;
                    parseBookJson(json_body, title, author, isbn);
                    
                    if (manager.updateBook(id, title, author, isbn)) {
                        Book* updated_book = manager.getBook(id);
                        response = createHttpResponse(bookToJson(*updated_book));
                    } else {
                        response = createHttpResponse("{\"error\":\"Book not found\"}", 400);
                    }
                }
            }
            // DELETE /api/books/:id - 删除图书
            else if (method == "DELETE" && path.find("/api/books/") == 0) {
                int id = std::stoi(path.substr(11));
                if (manager.deleteBook(id)) {
                    response = createHttpResponse("{\"success\":true}");
                } else {
                    response = createHttpResponse("{\"error\":\"Book not found\"}", 400);
                }
            }
            // POST /api/books/:id/borrow - 借阅图书
            else if (method == "POST" && path.find("/api/books/") != std::string::npos && 
                    path.find("/borrow") != std::string::npos) {
                int id = std::stoi(path.substr(11));
                size_t body_start = request.find("\r\n\r\n");
                std::string borrower = "Anonymous";
                
                if (body_start != std::string::npos) {
                    std::string json_body = request.substr(body_start + 4);
                    size_t borrower_pos = json_body.find("\"borrower\":\"");
                    if (borrower_pos != std::string::npos) {
                        size_t start = borrower_pos + 12;
                        size_t end = json_body.find("\"", start);
                        borrower = json_body.substr(start, end - start);
                    }
                }
                
                if (manager.borrowBook(id, borrower)) {
                    Book* book = manager.getBook(id);
                    response = createHttpResponse(bookToJson(*book));
                } else {
                    response = createHttpResponse("{\"error\":\"Cannot borrow book\"}", 400);
                }
            }
            // POST /api/books/:id/return - 归还图书
            else if (method == "POST" && path.find("/api/books/") != std::string::npos && 
                    path.find("/return") != std::string::npos) {
                int id = std::stoi(path.substr(11));
                if (manager.returnBook(id)) {
                    Book* book = manager.getBook(id);
                    response = createHttpResponse(bookToJson(*book));
                } else {
                    response = createHttpResponse("{\"error\":\"Cannot return book\"}", 400);
                }
            }
            else {
                response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            }
            
            send(client_socket, response.c_str(), response.length(), 0);
        }
        
        closesocket(client_socket);
    }
    
    closesocket(server_socket);
    return 0;
}
