from nicegui import ui
from nicegui.events import ValueChangeEventArguments, KeyEventArguments
from tcp_client import wifi_server, set_message, send_message, start_server_thread

##############################################################################################################################
# 基本定义部分
##############################################################################################################################

servo_min = -90
servo_max = 90
motor_min = -499
motor_max = 499
spin_min = -100
spin_max = 100

def limit_value(val, min, max):
    if val < min:
        val = min
    elif val > max:
        val = max
    if val >= 0:
        return "+" + str(val)
    else:
        return str(val)

class ServerStatus:
    def __init__(self):
        self.current_message = "I got you"
        self.server_status = "未启动"
        self.connection_status = "未连接"
        self.started = 0

server = ServerStatus()

def show(event: ValueChangeEventArguments):
    name = type(event.sender).__name__
    ui.notify(f'{name}: {event.value}')

class ControlData:
    def __init__(self):
        self.servo1 = 0
        self.servo2 = 0
        self.servo3 = 0
        self.servo4 = 0
        self.servo5 = 0
        self.servo6 = 0
        self.servo7 = 0
        self.servo8 = 0
        self.servo9 = 0
        self.servo10 = 0
        self.servo11 = 0
        self.servo12 = 0
        self.servo13 = 0
        self.servo14 = 0
        self.servo15 = 0
        self.servo16 = 0
        self.motor1 = 0
        self.motor2 = 0
        self.motor3 = 0
        self.motor4 = 0
        self.speed_v = 0
        self.speed_h = 0
        self.spin = 0
        self.mode = "line"
        self.motor_value = True
        self.motor_slider = False
        self.slider_consecutive = False
        self.slider_keyboard = False
        self.scan = False
    
    def reset_values(self):
        self.servo1 = 0
        self.servo2 = 0
        self.servo3 = 0
        self.servo4 = 0
        self.servo5 = 0
        self.servo6 = 0
        self.servo7 = 0
        self.servo8 = 0
        self.servo9 = 0
        self.servo10 = 0
        self.servo11 = 0
        self.servo12 = 0
        self.servo13 = 0
        self.servo14 = 0
        self.servo15 = 0
        self.servo16 = 0
        self.motor1 = 0
        self.motor2 = 0
        self.motor3 = 0
        self.motor4 = 0
        self.speed_v = 0
        self.speed_h = 0
        self.spin = 0
        self.slider_consecutive = False
        self.slider_keyboard = False
        self.scan = False

data = ControlData()

class KeyBoardData:
    def __init__(self):
        self.w = False
        self.a = False
        self.s = False
        self.d = False
        self.ArrowLeft = False
        self.ArrowRight = False
        self.active_keys = []
    def reset_keys(self):
        self.w = False
        self.a = False
        self.s = False
        self.d = False
        self.ArrowLeft = False
        self.ArrowRight = False
        self.active_keys = []

keydata = KeyBoardData()
Keys_name = ['w', 'a', 's', 'd', 'ArrowLeft', 'ArrowRight']

##############################################################################################################################
# 网络连接相关部分
##############################################################################################################################

def update_server_status():
    """更新服务器状态"""
    server.server_status = "已启动" if wifi_server.running else "未启动"
    server.connection_status = "已连接" if wifi_server.connected else "未连接"

def on_message_update(new_message: str):
    """消息更新回调"""
    server.current_message = new_message

def ui_set_value(event: ValueChangeEventArguments):
    value = event.value
    set_message(value)
    on_message_update(value)

# 添加定时器更新状态
def update_status():
    update_server_status()
ui.timer(0.5, update_status)

# 初始状态更新
update_server_status()

# 启动网络连接
def ui_start_connect():
    if server.started == 0 and not wifi_server.running:
        start_server_thread()
        server.started = 1
ui.timer(1.0, ui_start_connect)

# 紧急制动
def ui_ESTOP():
    value = "ESTOP"
    set_message(value)
    on_message_update(value)
    send_message()
    data.reset_values()

# 切换模式
def ui_switch_mode(mode1, mode2):
    value = mode1 + " to " + mode2
    set_message(value)
    on_message_update(value)
    send_message()

# SEND按钮
def ui_SEND():
    if data.motor_value == True:
        for i in range(1, 17):
            attr_name = f"servo{i}"
            current_value = getattr(data, attr_name)
            setattr(data, attr_name, limit_value(float(current_value), servo_min, servo_max))
        for i in range(1, 5):
            attr_name = f"motor{i}"
            current_value = getattr(data, attr_name)
            setattr(data, attr_name, limit_value(float(current_value), motor_min, motor_max))
        value = str(data.mode) + " " + str("value") + " " + str(data.motor1) + " " + str(data.motor2) + " " + str(data.motor3) + " " + str(data.motor4) \
         + " " + str(data.servo1) + " " + str(data.servo2) + " " + str(data.servo3) + " " + str(data.servo4) \
         + " " + str(data.servo5) + " " + str(data.servo6) + " " + str(data.servo7) + " " + str(data.servo8) \
         + " " + str(data.servo9) + " " + str(data.servo10) + " " + str(data.servo11) + " " + str(data.servo12) \
         + " " + str(data.servo13) + " " + str(data.servo14) + " " + str(data.servo15) + " " + str(data.servo16)
        set_message(value)
        on_message_update(value)
        send_message()
    elif data.motor_slider == True and data.slider_keyboard == False:
        value = str(data.mode) + " " + str("slider") + " " + limit_value(data.speed_h, spin_min, spin_max) + " " + limit_value(data.speed_v, spin_min, spin_max) \
            + " " + limit_value(data.spin, spin_min, spin_max)
        set_message(value)
        on_message_update(value)
        send_message()
    else:
        h = 0
        v = 0
        s = 0
        if keydata.s:
            v = -data.speed_v
        if keydata.w:
            v = data.speed_v
        if keydata.a:
            h = -data.speed_h
        if keydata.d:
            h = data.speed_h
        if keydata.ArrowLeft:
            s = -data.spin
        if keydata.ArrowRight:
            s = data.spin
        value = str(data.mode) + " " + str("slider") + " " + limit_value(h, spin_min, spin_max) + " " + limit_value(v, spin_min, spin_max) \
              + " " + limit_value(s, spin_min, spin_max)
        # print(h, v, s)
        set_message(value)
        on_message_update(value)
        send_message()

# 创建状态变量
is_active = False
# 创建定时器对象
timer = None
# slider连续控制开关
def ui_slider_consecutive(event: ValueChangeEventArguments):
    value = event.value
    """处理开关状态变化"""
    global is_active, timer
    is_active = value
    
    if is_active:
        # 启动定时器
        timer = ui.timer(0.1, ui_SEND, active=True)
        ui.notify("已启动实时模式")
    else:
        # 停止定时器
        if timer:
            timer.deactivate()
            ui.notify("已停止实时模式")
        data.slider_keyboard = False

# 恢复运行状态按钮
def ui_resume():
    data.reset_values()
    ui_SEND()
    value = "RESUME"
    set_message(value)
    on_message_update(value)
    send_message()
    ui.notify("已申请恢复运行")


##############################################################################################################################
# UI部分
##############################################################################################################################

# 在页面顶部放置一张图片，左右顶满页面
with ui.card().classes('w-full'):
    ui.image('https://notes.sjtu.edu.cn/uploads/upload_4421d34b3ad5f629a3bc6ec042c7eae8.png').classes('w-full')

ui.image('https://notes.sjtu.edu.cn/uploads/upload_c6519fc9c8fc35aafe9971b1f33a113a.png').style('position: absolute; left: 2.8%; top: -4.5%; width: 200px')

# 舵机位置（16个）
positions_servo = [
    (0.068, 0.265), (0.152, 0.265), (0.194, 0.265), (0.236, 0.265), (0.294, 0.265),
    (0.337, 0.265), (0.374, 0.265), (0.443, 0.265), (0.516, 0.265), (0.585, 0.265),
    (0.621, 0.265), (0.664, 0.265), (0.722, 0.265), (0.764, 0.265), (0.806, 0.265), (0.891, 0.265)
]

#电机位置（4个)
positions_motor = [(0.130, 0.147), (0.381, 0.147), (0.575, 0.147), (0.825, 0.147)]

# 显示舵机控制值
for i in range (0, len(positions_servo)):
    x, y = positions_servo[i]
    with ui.card().style(f'position: absolute; left: {x*100}%; top: {y*100}%; background-color: transparent; box-shadow: none;').bind_visibility_from(data, 'motor_value'):
        input = ui.input('').props('border none').style('text-align: center; width: 30px; height: 15px; border: none; background-color: transparent;')
        input.bind_value(data, f'servo{i + 1}')

#显示电机控制值
for i in range (0, len(positions_motor)):
    x, y = positions_motor[i]
    with ui.card().style(f'position: absolute; left: {x*100}%; top: {y*100}%; background-color: transparent; box-shadow: none;').bind_visibility_from(data, 'motor_value'):
        input = ui.input('').props('border none').style('text-align: center; width: 30px; height: 15px; border: none; background-color: transparent;')
        input.bind_value(data, f'motor{i + 1}')

#车辆模式选择按钮
# 创建自定义样式的 Toggle 组件
with ui.card().style("width: 150px; height: 75px; padding: 0; border: 1px solid #ccc; position: absolute; left: 6%; top: 68%;"):
    # 使用行布局排列两个按钮
    with ui.row().classes("w-full h-full no-wrap gap-0"):
        # Line 按钮 - 使用深蓝色背景和黑色文字
        line_btn = ui.button("Line", on_click=lambda: select_mode("line")) \
            .classes("w-1/2 h-full rounded-none") \
            .props("flat") \
            .style("font-size: 16px; font-weight: bold; border-radius: 0; color: black;")
        
        # Car 按钮 - 使用浅蓝色背景和黑色文字
        car_btn = ui.button("Car", on_click=lambda: select_mode("car")) \
            .classes("w-1/2 h-full rounded-none") \
            .props("flat") \
            .style("font-size: 16px; font-weight: bold; border-radius: 0; color: black;")

# 设置按钮样式函数
def set_button_style(selected_btn, unselected_btn):
    """设置按钮样式，选中按钮使用深蓝色，未选中按钮使用浅蓝色"""
    selected_btn.style("background-color: #1976d2 !important; color: black !important;")
    unselected_btn.style("background-color: #bbdefb !important; color: black !important;")

# 初始设置样式
set_button_style(line_btn, car_btn)

# 选择模式函数
def select_mode(mode):
    last_mode = str(data.mode)
    data.mode = mode
    
    if mode == "line":
        set_button_style(line_btn, car_btn)
        if last_mode != "line":
            data.scan = False
            ui_switch_mode("car", "line")
    else:
        set_button_style(car_btn, line_btn)
        if last_mode != "car":
            data.scan = False
            ui_switch_mode("line", "car")
    
    ui.notify(f"已选择车辆模式: {mode.upper()}")

#电机控制模式选择按钮
# 创建自定义样式的 Toggle 组件
with ui.card().style("width: 150px; height: 75px; padding: 0; border: 1px solid #ccc; position: absolute; left: 6%; top: 81%;"):
    # 使用行布局排列两个按钮
    with ui.row().classes("w-full h-full no-wrap gap-0"):
        # Line 按钮 - 使用深蓝色背景和黑色文字
        motor_value_btn = ui.button("Value", on_click=lambda: select_mode_motor("value")) \
            .classes("w-1/2 h-full rounded-none") \
            .props("flat") \
            .style("font-size: 16px; font-weight: bold; border-radius: 0; color: black;")
        
        # Car 按钮 - 使用浅蓝色背景和黑色文字
        motor_slider_btn = ui.button("Slider", on_click=lambda: select_mode_motor("slider")) \
            .classes("w-1/2 h-full rounded-none") \
            .props("flat") \
            .style("font-size: 16px; font-weight: bold; border-radius: 0; color: black;")
        
# 初始设置样式
set_button_style(motor_value_btn, motor_slider_btn)
        
def select_mode_motor(mode):
    last_motor_value = str(data.motor_value)
    last_motor_slider = str(data.motor_slider)

    if mode == "value":
        data.motor_value = True
        data.motor_slider = False
        data.slider_consecutive = False
        set_button_style(motor_value_btn, motor_slider_btn)
        if last_motor_value != "True":
            data.scan = False
            ui_switch_mode("slider", "value")
    else:
        data.motor_value = False
        data.motor_slider = True
        set_button_style(motor_slider_btn, motor_value_btn)
        if last_motor_slider != "True":
            ui_switch_mode("value", "slider")

    ui.notify(f"已选择电机控制模式: {mode.upper()}")


# 创建 SEND 按钮
send_button = ui.button("SEND") \
    .style("""
        width: 150px;
        height: 75px;
        color: black !important;
        background-color: #4CAF50 !important;  /* 绿色背景 */
        font-size: 24px;
        font-weight: bold;
        border-radius: 8px;
        box-shadow: 0 4px 8px rgba(0,0,0,0.2);
        transition: all 0.3s;
        position: absolute;
        left: 21%; top: 68%;
    """) \
    .props("flat") \
    .on("mouseover", lambda: send_button.style("box-shadow: 0 6px 12px rgba(0,0,0,0.3); transform: translateY(-2px);")) \
    .on("mouseout", lambda: send_button.style("box-shadow: 0 4px 8px rgba(0,0,0,0.2); transform: none;"))

# 添加点击效果
send_button.on("click", lambda: on_send_click())
def on_send_click():
    # 添加点击动画
    send_button.style("transform: scale(0.95);")
    ui.timer(0.1, lambda: send_button.style("transform: none;"), once=True)
    
    # 执行发送操作
    ui_SEND()
    ui.notify("发送操作已执行!")



# 急停按钮
def on_stop_click():
    """STOP按钮点击事件处理"""
    ui_ESTOP()
    ui.notify("紧急停止已触发!", type="negative")

# 创建STOP按钮
stop_btn = ui.button("STOP!", on_click=on_stop_click) \
    .style("""
        width: 150px;
        height: 75px;
        font-size: 32px;
        font-weight: bold;
        color: black !important;
        background-color: #ff4444 !important;
        border: 2px solid #cc0000 !important;
        border-radius: 8px;
        box-shadow: 0 4px 8px rgba(0,0,0,0.2);
        position: absolute;
        left: 21%; top: 81%;
    """) \
    .props("unelevated")  # 移除默认阴影效果

# 添加悬停和点击效果
stop_btn.on('mouseover', lambda: stop_btn.style("background-color: #ff0000 !important;"))
stop_btn.on('mouseout', lambda: stop_btn.style("background-color: #ff4444 !important;"))
stop_btn.on('mousedown', lambda: stop_btn.style("background-color: #cc0000 !important; transform: translateY(2px);"))
stop_btn.on('mouseup', lambda: stop_btn.style("background-color: #ff4444 !important; transform: translateY(0);"))


#车辆电机控制滑块
# 创建主容器
with ui.card().classes('w-full max-w-xl p-3').style('position: absolute; left: 63.5%; top: 41.5%; width: 500px; height: 400px; background-color: transparent; box-shadow: none;').bind_visibility_from(data, 'motor_slider'):
    # # 标题
    # ui.label('车辆速度控制').classes('text-2xl font-bold mb-8 text-center')
    
    # 使用网格布局放置滑块
    with ui.grid(columns=2).classes('w-full gap-5'):
        # 左侧列：横向和旋转控制
        with ui.column().classes('items-center gap-8'):
            # 左右速度控制 (横向滑块)
            with ui.column().classes('items-center w-full'):
                ui.label('左右速度').classes('text-lg font-medium-bold mb-4')
                
                # 横向滑块和数值输入框容器
                with ui.row().classes('items-center gap-4 w-full justify-center'):
                    # 创建横向滑块
                    horizontal_slider = ui.slider(min=-100, max=100, value=0, step=1) \
                        .props('') \
                        .classes('w-full max-w-xs') \
                        .style('background-color: #f0f0f0') \
                        .bind_value(data, 'speed_h')
                    
                    # 数值输入框
                    horizontal_input = ui.number(
                        label='数值', 
                        min=-100, max=100, step=1, 
                        # format='%.1f'
                    ).classes('w-32') \
                        .bind_value_from(data, 'speed_h') \
                        .bind_value_to(data, 'speed_h')
            
            # 旋转控制 (新增滑块)
            with ui.column().classes('items-center w-full'):
                ui.label('旋转控制').classes('text-lg font-medium-bold mb-4')
                
                # 旋转滑块和数值输入框容器
                with ui.row().classes('items-center gap-4 w-full justify-center'):
                    # 创建旋转滑块
                    rotation_slider = ui.slider(min=-100, max=100, value=0, step=1) \
                        .props('') \
                        .classes('w-full max-w-xs') \
                        .style('background-color: #f0f0f0') \
                        .bind_value(data, 'spin')
                    
                    # 数值输入框
                    rotation_input = ui.number(
                        label='数值', 
                        min=-100, max=100, step=1, 
                        # format='%.1f'
                    ).classes('w-32') \
                        .bind_value_from(data, 'spin') \
                        .bind_value_to(data, 'spin')
        
        # 右侧列：前后控制
        with ui.column().classes('items-center'):
            ui.label('前后速度').classes('text-lg font-medium-bold mb-4')
            
            # 创建垂直滑块容器
            with ui.row().classes('h-64 items-center justify-center'):
                # 创建竖向滑块
                vertical_slider = ui.slider(min=-100, max=100, value=0, step=1) \
                    .props('vertical') \
                    .classes('h-60 vertical') \
                    .style('background-color: #f0f0f0') \
                    .bind_value(data, 'speed_v')
                
                # 数值输入框容器
                with ui.column().classes('ml-4 gap-2'):
                    # 显示滑块值
                    ui.label('当前值:').classes('font-medium')
                    vertical_input = ui.number(
                        label='数值', 
                        min=-100, max=100, step=1, 
                        # format='%.1f'
                    ).classes('w-32') \
                        .bind_value_from(data, 'speed_v') \
                        .bind_value_to(data, 'speed_v')

ui.button('重置', on_click=lambda: [
    horizontal_slider.set_value(0),
    vertical_slider.set_value(0),
    rotation_slider.set_value(0)
]).props('outline').classes('px-16 py-4').style('position: absolute; left: 84%; top: 84%; font-size: 32px').bind_visibility_from(data, 'motor_slider')
ui.switch('实时控制', on_change=ui_slider_consecutive).classes('font-medium').style('position: absolute; left: 87%; top: 78%;').bind_visibility_from(data, 'motor_slider').bind_value(data, "slider_consecutive")

# 创建键盘事件处理
def handle_key_event(e: KeyEventArguments):
    """处理键盘事件"""
    key = e.key.name
    off = e.action.keyup
    on = e.action.keydown
    # print(key, type(key))
    if key in Keys_name:
        if getattr(keydata, key) == False and on:
            setattr(keydata, key, True)
        elif getattr(keydata, key) == True and off:
            setattr(keydata, key, False)

# 添加键盘监听
ui.keyboard(on_key=handle_key_event)

# 键盘操作开关
ui.switch('键盘操作').classes('font-medium').style('position: absolute; left: 87%; top: 74%;').bind_visibility_from(data, 'slider_consecutive').bind_value(data, "slider_keyboard")

def change_scan():
    if data.scan == True:
        ui_switch_mode(f"{data.mode}", "scan")
        ui.notify("已切换至扫描模式")
    else:
        ui_switch_mode("scan", f"{data.mode}")
        ui.notify("已退出扫描模式")
# 扫描模式开关
ui.switch(on_change=change_scan).classes('transform -rotate-90 scale-150').style('position: absolute; left: 92%; top: 56%;').bind_visibility_from(data, 'motor_slider').bind_value(data, 'scan')
ui.label('SCAN').classes('transform -rotate-90 font-bold').style('position: absolute; left: 95%; top: 57%;').bind_visibility_from(data, 'motor_slider')


# 更新当前按下字母显示
def update_active_keys():
    keydata.active_keys = [key for key in Keys_name if getattr(keydata, key)]
ui.timer(0.1, update_active_keys)

active_label = ui.label().bind_text(keydata, "active_keys").style('position: absolute; left: 67%; top: 92%;').bind_visibility(data, 'slider_keyboard')

# 基本信息模块
with ui.card().classes('max-w-2xl p-6').style('position: absolute; left: 4.5%; top: 41.5%; width: 500px; background-color: transparent; box-shadow: none'):
    # 服务器状态显示
    with ui.row().classes('w-full justify-between items-center mb-3'):
        ui.label('猫猫蛇 TCP 客户端控制面板').classes('text-2xl font-bold mb-3 text-center')
        
        ui.label().bind_text_from(server, 'server_status') \
            .classes('text-lg font-semibold')
            
        ui.label().bind_text_from(server, 'connection_status') \
            .classes('text-lg font-semibold')
        
with ui.card().classes('text-lg font-semibold').style('position: absolute; left: 5%; top: 51.5%; width: 1300px; background-color: transparent; box-shadow: none'):
    # ui.label('连接信息').classes('text-xl font-bold mb-4')
    with ui.row().classes('justify-between items-center mb-3'):
        ui.label('服务器地址:').classes('text-right font-medium-bold')
        ui.label(f'{wifi_server.host}:{wifi_server.port}').classes('font-bold')
    
    with ui.row().classes('justify-between items-center mb-3'):
        ui.label('上一条消息:').classes('text-right font-medium-bold')
        ui.label().bind_text_from(server, 'current_message').classes('font-bold')

ui.button('RESUME', on_click=ui_resume).props('outline').classes('px-10 py-3 font-bold').style('position: absolute; left: 28.25%; top: 52.5%;')

ui.run(port=8081, favicon='🐍', title='猫猫蛇TCP客户端')  