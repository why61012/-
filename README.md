# 图书馆图书管理系统 📚

一个简单的图书馆图书管理系统演示项目，包含 C++ 后端 API 和前端 Web 界面。

## 功能特性

- ✅ 图书增删改查 (CRUD)
- ✅ 图书借阅/归还管理
- ✅ 实时搜索和筛选
- ✅ 数据内存存储（无需数据库）
- ✅ RESTful API
- ✅ 响应式 Web 界面

## 项目结构

```
.
├── backend/           # C++ 后端（CLion）
│   ├── CMakeLists.txt
│   ├── src/
│   │   └── main.cpp
│   └── include/
│       └── library.h
└── frontend/          # 前端（WebStorm）
    ├── index.html
    ├── style.css
    └── script.js
```

## 快速开始

### 后端（C++ - CLion）

1. 使用 CLion 打开 `backend/` 文件夹
2. CMake 会自动配置项目
3. 编译运行 main.cpp
4. 服务器将在 `http://localhost:8080` 启动

### 前端（WebStorm）

1. 使用 WebStorm 打开 `frontend/` 文件夹
2. 右键点击 `index.html` 选择 "Open in Browser"
3. 或在浏览器中打开 `frontend/index.html`
4. 连接到后端 API（http://localhost:8080）

## API 端点

| 方法 | 端点 | 描述 |
|------|------|------|
| GET | /api/books | 获取所有图书 |
| POST | /api/books | 添加新图书 |
| PUT | /api/books/:id | 更新图书 |
| DELETE | /api/books/:id | 删除图书 |
| POST | /api/books/:id/borrow | 借阅图书 |
| POST | /api/books/:id/return | 归还图书 |

## 使用示例

### 获取所有图书
```bash
curl http://localhost:8080/api/books
```

### 添加新图书
```bash
curl -X POST http://localhost:8080/api/books \
  -H "Content-Type: application/json" \
  -d '{"title":"The Great Gatsby","author":"F. Scott Fitzgerald","isbn":"978-0-7432-7356-5"}'
```

### 借阅图书
```bash
curl -X POST http://localhost:8080/api/books/1/borrow
```

## 技术栈

- **后端**: C++17, HTTP 服务器
- **前端**: HTML5, CSS3, Vanilla JavaScript
- **API**: RESTful JSON
- **存储**: 内存数据结构

## 注意事项

- 这是一个演示项目，数据存储在内存中
- 重启服务器后所有数据将丢失
- 适合学习和演示使用

## 作者

Created for library management system demonstration.
