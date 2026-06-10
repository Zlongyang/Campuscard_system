# 🎓 校园卡管理系统 (Campus Card Management System)

一个基于 C 语言和 Web 技术的校园一卡通管理系统，支持学生管理、校园卡管理、交易记录查询等功能。

## 功能特性

### 管理员端
- 📊 **系统概览** — 实时统计数据
- 👥 **学生管理** — 添加、编辑、删除、搜索学生
- 💳 **校园卡管理** — 办卡、冻结、解冻、挂失、注销、充值
- 📋 **交易记录** — 查看/搜索所有交易记录
- 💰 **手动充值** — 为校园卡充值

### 学生端
- 🏠 **我的校园卡** — 查看卡余额、状态、个人信息
- 📋 **交易记录** — 查看个人消费/充值记录
- 🍽️ **消费模拟** — 模拟食堂/超市刷卡消费
- 🔒 **修改密码** — 修改登录密码

## 技术架构

```
├── include/          # 头文件
│   ├── card_data.h   # 核心数据模型定义
│   ├── file_io.h     # 文件读写层
│   ├── utils.h       # 工具函数
│   ├── login.h       # 登录认证与会话管理
│   ├── admin_op.h    # 管理员操作 API
│   ├── student_op.h  # 学生操作 API
│   └── server.h      # HTTP 服务器
├── src/              # C 源文件实现
│   ├── main.c        # 程序入口
│   ├── server.c      # Mongoose HTTP 服务器 + API 路由
│   ├── card_data.c   # 核心业务逻辑
│   ├── file_io.c     # 文件读写实现
│   ├── utils.c       # 工具函数实现
│   ├── login.c       # 登录/会话管理
│   ├── admin_op.c    # 管理员业务
│   └── student_op.c  # 学生业务
├── web/              # 前端页面
│   ├── index.html    # 自动跳转页
│   ├── login.html    # 登录页
│   ├── admin.html    # 管理后台
│   ├── student.html  # 学生中心
│   ├── api.js        # 前端 API 封装
│   └── style.css     # 统一样式
├── data/             # 数据存储（文本文件）
│   ├── students.txt  # 学生数据 (管道分隔)
│   ├── cards.txt     # 校园卡数据
│   ├── records.txt   # 交易记录
│   └── next_ids.dat  # ID 计数器
├── thirdparty/       # 第三方库
│   ├── mongoose.c    # Mongoose HTTP 服务器
│   └── mongoose.h
├── CampusCardSys.sln
└── CampusCardSys.vcxproj
```

## 数据格式

所有数据以管道符 `|` 分隔的文本文件存储：

```
# students.txt
id|student_no|name|department|grade|phone|password_hash

# cards.txt
id|card_no|student_no|name|balance|status|daily_limit|issue_date

# records.txt
id|card_no|student_no|type|amount|balance_before|balance_after|time|description
```

## 编译运行

### Windows (Visual Studio)
直接在 Visual Studio 中打开 `CampusCardSys.sln`，编译运行。

### Windows (MinGW/GCC)
```bash
gcc -Wall -Wextra -I. -Ithirdparty src/*.c thirdparty/mongoose.c -o CampusCardSys.exe -lws2_32
CampusCardSys.exe 8080
```

### Linux/Mac (GCC)
```bash
gcc -Wall -Wextra -I. -Ithirdparty src/*.c thirdparty/mongoose.c -o CampusCardSys -lpthread
./CampusCardSys 8080
```

## 默认账号

| 角色 | 账号 | 密码 |
|------|------|------|
| 管理员 | admin | admin123 |
| 学生 | 学号 (如 STU20240001) | 123456（默认） |

## API 接口

| 方法 | 路径 | 说明 | 权限 |
|------|------|------|------|
| POST | /api/login | 登录 | 公开 |
| POST | /api/logout | 退出 | 登录 |
| GET | /api/students | 学生列表/搜索 | 管理员 |
| POST | /api/student/add | 添加学生 | 管理员 |
| POST | /api/student/edit | 编辑学生 | 管理员 |
| POST | /api/student/delete | 删除学生 | 管理员 |
| GET | /api/cards | 卡列表/搜索 | 管理员 |
| POST | /api/card/issue | 办理新卡 | 管理员 |
| POST | /api/card/freeze | 冻结卡 | 管理员 |
| POST | /api/card/unfreeze | 解冻卡 | 管理员 |
| POST | /api/card/report-lost | 挂失卡 | 管理员 |
| POST | /api/card/cancel | 注销卡 | 管理员 |
| POST | /api/card/recharge | 充值 | 管理员 |
| POST | /api/card/consume | 消费 | 登录 |
| GET | /api/records | 交易记录 | 管理员 |
| GET | /api/my-card | 我的卡信息 | 学生 |
| GET | /api/my-records | 我的交易记录 | 学生 |
| GET | /api/my-info | 我的个人信息 | 学生 |
| POST | /api/change-pwd | 修改密码 | 学生 |
| GET | /api/stats | 系统统计 | 管理员 |

## 第三方库

- [Mongoose](https://github.com/cesanta/mongoose) — 嵌入式 HTTP/WebSocket 服务器

## License

MIT
