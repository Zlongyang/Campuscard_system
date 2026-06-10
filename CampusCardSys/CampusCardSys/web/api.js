/**
 * api.js — 校园卡系统前端 API 封装
 * 所有后端请求的 JS 封装函数
 */

const API_BASE = '';

/* ========== 通用请求 ========== */

let AUTH_TOKEN = localStorage.getItem('token') || '';

function setToken(token) {
    AUTH_TOKEN = token;
    localStorage.setItem('token', token);
}

function clearToken() {
    AUTH_TOKEN = '';
    localStorage.removeItem('token');
    localStorage.removeItem('role');
    localStorage.removeItem('student_no');
    localStorage.removeItem('account');
}

async function apiRequest(url, method = 'GET', body = null) {
    const opts = {
        method: method,
        headers: { 'Content-Type': 'application/json' }
    };
    if (AUTH_TOKEN) {
        opts.headers['Authorization'] = 'Bearer ' + AUTH_TOKEN;
    }
    if (body) {
        opts.body = JSON.stringify(body);
    }

    try {
        const resp = await fetch(API_BASE + url, opts);
        const data = await resp.json();
        return data;
    } catch (err) {
        return { code: 500, msg: '网络错误: ' + err.message, data: null };
    }
}

/* ========== 登录 API ========== */

async function apiLogin(account, password, role) {
    const data = await apiRequest('/api/login', 'POST', { account, password, role });
    if (data.code === 200 && data.data && data.data.token) {
        setToken(data.data.token);
        localStorage.setItem('role', data.data.role);
        localStorage.setItem('account', data.data.account);
        if (data.data.student_no) {
            localStorage.setItem('student_no', data.data.student_no);
        }
    }
    return data;
}

async function apiLogout() {
    await apiRequest('/api/logout', 'POST');
    clearToken();
}

/* ========== 学生管理 API（管理员） ========== */

async function apiGetStudents(keyword) {
    const params = keyword ? '?keyword=' + encodeURIComponent(keyword) : '';
    return await apiRequest('/api/students' + params);
}

async function apiAddStudent(student) {
    return await apiRequest('/api/student/add', 'POST', student);
}

async function apiEditStudent(student) {
    return await apiRequest('/api/student/edit', 'POST', student);
}

async function apiDeleteStudent(studentNo) {
    return await apiRequest('/api/student/delete', 'POST', { student_no: studentNo });
}

/* ========== 校园卡管理 API（管理员） ========== */

async function apiGetCards(keyword) {
    const params = keyword ? '?keyword=' + encodeURIComponent(keyword) : '';
    return await apiRequest('/api/cards' + params);
}

async function apiIssueCard(studentNo, dailyLimit) {
    return await apiRequest('/api/card/issue', 'POST', {
        student_no: studentNo,
        daily_limit: dailyLimit || 500
    });
}

async function apiCardFreeze(cardNo) {
    return await apiRequest('/api/card/freeze', 'POST', { card_no: cardNo });
}

async function apiCardUnfreeze(cardNo) {
    return await apiRequest('/api/card/unfreeze', 'POST', { card_no: cardNo });
}

async function apiCardReportLost(cardNo) {
    return await apiRequest('/api/card/report-lost', 'POST', { card_no: cardNo });
}

async function apiCardCancel(cardNo) {
    return await apiRequest('/api/card/cancel', 'POST', { card_no: cardNo });
}

async function apiCardRecharge(cardNo, amount, desc) {
    return await apiRequest('/api/card/recharge', 'POST', {
        card_no: cardNo,
        amount: parseFloat(amount),
        description: desc || '充值'
    });
}

async function apiCardConsume(cardNo, amount, desc) {
    return await apiRequest('/api/card/consume', 'POST', {
        card_no: cardNo,
        amount: parseFloat(amount),
        description: desc || '消费'
    });
}

/* ========== 交易记录 API ========== */

async function apiGetRecords(params) {
    let query = '';
    if (params) {
        const parts = [];
        if (params.card_no) parts.push('card_no=' + encodeURIComponent(params.card_no));
        if (params.student_no) parts.push('student_no=' + encodeURIComponent(params.student_no));
        if (params.keyword) parts.push('keyword=' + encodeURIComponent(params.keyword));
        if (parts.length) query = '?' + parts.join('&');
    }
    return await apiRequest('/api/records' + query);
}

/* ========== 统计 API ========== */

async function apiGetStats() {
    return await apiRequest('/api/stats');
}

/* ========== 学生端 API ========== */

async function apiGetMyCard() {
    return await apiRequest('/api/my-card');
}

async function apiGetMyRecords() {
    return await apiRequest('/api/my-records');
}

async function apiGetMyInfo() {
    return await apiRequest('/api/my-info');
}

async function apiChangePassword(oldPwd, newPwd) {
    return await apiRequest('/api/change-pwd', 'POST', {
        old_password: oldPwd,
        new_password: newPwd
    });
}

/* ========== 工具函数 ========== */

function getRole() {
    return parseInt(localStorage.getItem('role') || '-1');
}

function isLoggedIn() {
    return !!AUTH_TOKEN;
}

function requireAuth() {
    if (!isLoggedIn()) {
        window.location.href = 'login.html';
        return false;
    }
    return true;
}

function requireRole(role) {
    if (!requireAuth()) return false;
    if (getRole() !== role) {
        alert('无权限访问此页面');
        if (getRole() === 0) window.location.href = 'admin.html';
        else if (getRole() === 1) window.location.href = 'student.html';
        else window.location.href = 'login.html';
        return false;
    }
    return true;
}

/* 金额格式化 */
function formatMoney(val) {
    return '¥' + parseFloat(val).toFixed(2);
}

/* 时间格式化 */
function formatTime(ts) {
    if (!ts) return '--';
    return ts;
}

/* 卡状态标签 */
function statusBadge(status) {
    const map = {
        0: '<span class="badge badge-success">正常</span>',
        1: '<span class="badge badge-warning">冻结</span>',
        2: '<span class="badge badge-danger">挂失</span>',
        3: '<span class="badge badge-secondary">注销</span>'
    };
    return map[status] || '<span class="badge badge-secondary">未知</span>';
}

function recordTypeBadge(type) {
    const map = {
        0: '<span class="badge badge-info">充值</span>',
        1: '<span class="badge badge-warning">消费</span>',
        2: '<span class="badge badge-info">退款</span>'
    };
    return map[type] || '<span class="badge badge-secondary">未知</span>';
}

/* Toast 提示 */
function showToast(msg, type) {
    const toast = document.createElement('div');
    toast.className = 'toast toast-' + (type || 'info');
    toast.textContent = msg;
    document.body.appendChild(toast);
    setTimeout(function() { toast.classList.add('show'); }, 10);
    setTimeout(function() {
        toast.classList.remove('show');
        setTimeout(function() { document.body.removeChild(toast); }, 300);
    }, 3000);
}
