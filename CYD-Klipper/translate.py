# -*- coding: utf-8 -*-
import os

replacements = {
    '"Print file"': '"Печать файла"',
    '"Cancel"': '"Отмена"',
    '"Failed to read file."': '"Ошибка чтения файла."',
    '"Files"': '"Файлы"',
    '"Macros"': '"Макросы"',
    '"Move"': '"Оси"',
    '"Temperature"': '"Температура"',
    '"Settings"': '"Настройки"',
    '"Connecting..."': '"Подключение..."',
    '"Error"': '"Ошибка"',
    '"Stop"': '"Стоп"',
    '"Pause"': '"Пауза"',
    '"Resume"': '"Продолжить"',
    '"Extrude"': '"Экструзия"',
    '"Retract"': '"Откат"',
    '"Cooling"': '"Охлаждение"',
    '"Fan"': '"Вентилятор"',
    '"Home"': '"Домой"',
    '"Home All"': '"Домой всё"',
    '"Disable Steppers"': '"Откл. моторы"',
    '"Speed"': '"Скорость"',
    '"Flow"': '"Поток"',
    '"Z Offset"': '"Z Офсет"',
    '"Bed"': '"Стол"',
    '"Hotend"': '"Сопло"',
    '"Back"': '"Назад"'
}

directory = r'C:\Users\m\.gemini\antigravity\scratch\CYD-Klipper\CYD-Klipper\src\ui'

for root, dirs, files in os.walk(directory):
    for file in files:
        if file.endswith('.cpp') or file.endswith('.h'):
            filepath = os.path.join(root, file)
            with open(filepath, 'r', encoding='utf-8') as f:
                content = f.read()
            
            modified = content
            for eng, rus in replacements.items():
                modified = modified.replace(eng, rus)
            
            if modified != content:
                with open(filepath, 'w', encoding='utf-8') as f:
                    f.write(modified)
                print(f"Translated strings in {file}")
