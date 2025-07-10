# tcp_client.py
import socket
import time
import threading
from typing import Callable

# 定义要发送的消息
MESSAGE = "I got you"

# 消息更新回调函数
message_update_callback: Callable[[str], None] = None

class WiFiTCPServer:
    def __init__(self, host='192.168.1.108', port=8080):
        self.host = host
        self.port = port
        self.server_socket = None
        self.client_socket = None
        self.client_address = None
        self.running = False
        self.connected = False
        self.message = MESSAGE
        self.lock = threading.Lock()  # 添加线程锁
        
    def start_server(self):
        """启动TCP服务器"""
        # 尝试多个端口，以防当前端口被占用
        ports_to_try = [self.port, 8081, 8082, 8083, 9999]
        server_started = False
        
        for port in ports_to_try:
            try:
                self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                # 设置端口重用，避免"Address already in use"错误
                self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                self.server_socket.bind((self.host, port))
                self.server_socket.listen(1)
                self.running = True
                self.port = port  # 更新实际使用的端口
                server_started = True
                
                print(f"TCP服务器已启动，监听地址: {self.host}:{port}")
                if self.host == '0.0.0.0':
                    print("服务器监听所有网络接口，WiFi模块可以连接到本机的任意IP地址")
                print("等待WiFi模块连接...")
                break
                
            except socket.error as e:
                if self.server_socket:
                    self.server_socket.close()
                    self.server_socket = None
                if port == ports_to_try[-1]:  # 最后一个端口也失败了
                    print(f"所有端口都无法绑定，最后尝试的端口{port}错误: {e}")
                    print("请检查:")
                    print("1. 是否有其他程序占用了这些端口")
                    print("2. 是否需要管理员权限运行")
                    print("3. 防火墙是否阻止了端口访问")
                else:
                    print(f"端口 {port} 被占用，尝试下一个端口...")
                    continue
        
        if not server_started:
            return
            
        while self.running:
            try:
                # 等待客户端连接
                self.client_socket, self.client_address = self.server_socket.accept()
                self.connected = True
                print(f"WiFi模块已连接，地址: {self.client_address}")
                
                # 启动发送线程
                send_thread = threading.Thread(target=self.send_message)
                send_thread.daemon = True
                send_thread.start()
                
                # 启动接收线程
                receive_thread = threading.Thread(target=self.receive_messages)
                receive_thread.daemon = True
                receive_thread.start()
                
                # 等待连接断开
                send_thread.join()
                receive_thread.join()
                
            except socket.error as e:
                if self.running:
                    print(f"连接错误: {e}")
                break
                    
            except Exception as e:
                print(f"服务器启动失败: {e}")
            finally:
                # 仅清理客户端连接，保留服务器套接字
                self.cleanup_client()
    
    def send_message(self):
        with self.lock:
            """发送当前消息"""
            if not self.connected or not self.client_socket:
                print("未连接客户端，无法发送消息")
                return False
                
            try:
                # 等待连接稳定
                time.sleep(0.01)
                self.client_socket.send(self.message.encode('utf-8'))
                print(f"已发送: {self.message}")
                return True
            except socket.error as e:
                print(f"发送失败: {e}")
                self.connected = False
                return False
    
    def receive_messages(self):
        """接收来自WiFi模块的消息"""
        while self.connected and self.running:
            try:
                if self.client_socket:
                    data = self.client_socket.recv(1024)
                    if data:
                        message = data.decode('utf-8')
                        print(f"收到消息: {message}")
                    else:
                        # 客户端断开连接
                        print("WiFi模块断开连接")
                        self.connected = False
                        break
            except socket.error as e:
                print(f"接收失败: {e}")
                self.connected = False
                break
            except KeyboardInterrupt:
                print("\n用户中断接收")
                break
    
    def stop_server(self):
        """停止服务器"""
        self.running = False
        self.connected = False
        self.cleanup()
    
    def cleanup(self):
        """清理资源"""
        if self.client_socket:
            try:
                self.client_socket.close()
            except:
                pass
            self.client_socket = None
            
        if self.server_socket:
            try:
                self.server_socket.close()
            except:
                pass
            self.server_socket = None
        
        print("服务器已停止")

    def cleanup_client(self):
        """仅清理客户端资源"""
        if self.client_socket:
            try:
                self.client_socket.close()
            except:
                pass
            self.client_socket = None
            self.client_address = None
            self.connected = False
        print("客户端连接已清理，等待新连接...")

# 全局服务器实例
wifi_server = WiFiTCPServer(host='172.18.29.118', port=8080)

def start_server_thread():
    print("启动线程")
    """启动服务器线程"""
    if wifi_server.running:
        print("服务器已在运行中")
        return None
    """启动服务器线程"""
    server_thread = threading.Thread(target=wifi_server.start_server)
    server_thread.daemon = True
    server_thread.start()
    return server_thread

def set_message(new_message: str):
    """设置要发送的消息"""
    global MESSAGE
    MESSAGE = new_message
    wifi_server.message = new_message
    if message_update_callback:
        message_update_callback(new_message)

def send_message():
    """发送消息"""
    return wifi_server.send_message()

# # 测试代码
# if __name__ == "__main__":
#     start_server_thread()
#     try:
#         while True:
#             time.sleep(1)
#     except KeyboardInterrupt:
#         wifi_server.stop_server()