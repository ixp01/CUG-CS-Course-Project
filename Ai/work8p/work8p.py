import os
import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
from torch.utils.data import DataLoader, TensorDataset
import torchvision
from torchvision import datasets, transforms
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import PIL
from PIL import Image, ImageDraw

# 定义LeNet-5模型
class LeNet5(nn.Module):
    def __init__(self):
        super(LeNet5, self).__init__()
        self.conv1 = nn.Conv2d(1, 6, kernel_size=5, padding=2)  # 卷积层1
        self.pool1 = nn.MaxPool2d(kernel_size=2, stride=2)  # 池化层1
        self.conv2 = nn.Conv2d(6, 16, kernel_size=5)  # 卷积层2
        self.pool2 = nn.MaxPool2d(kernel_size=2, stride=2)  # 池化层2
        self.fc1 = nn.Linear(16 * 5 * 5, 120)  # 全连接层1
        self.fc2 = nn.Linear(120, 84)  # 全连接层2
        self.fc3 = nn.Linear(84, 10)  # 输出层
        
    def forward(self, x):
        x = self.pool1(F.relu(self.conv1(x)))
        x = self.pool2(F.relu(self.conv2(x)))
        x = x.view(-1, 16 * 5 * 5)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)
        return x

class DigitRecognitionApp:
    def __init__(self, root):
        self.root = root
        self.root.title("LeNet-5 手写数字识别")
        self.root.geometry("1000x600")
        self.root.resizable(True, True)
        
        # 创建模型
        self.model = None
        self.is_model_trained = False
        
        # 设置设备
        self.device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
        print(f"使用设备: {self.device}")
        
        # 创建数据变量
        self.train_loader = None
        self.test_loader = None
        self.history = {"train_loss": [], "train_acc": [], "val_loss": [], "val_acc": []}
        
        # 创建UI
        self.create_ui()
        
    def create_ui(self):
        # 创建左右框架
        left_frame = ttk.Frame(self.root, padding=10)
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        right_frame = ttk.Frame(self.root, padding=10)
        right_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)
        
        # 左侧 - 模型训练部分
        train_frame = ttk.LabelFrame(left_frame, text="模型训练", padding=10)
        train_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # 数据加载按钮
        self.load_btn = ttk.Button(train_frame, text="加载数据", command=self.load_data)
        self.load_btn.pack(fill=tk.X, pady=5)
        
        # 参数设置框架
        params_frame = ttk.LabelFrame(train_frame, text="训练参数", padding=10)
        params_frame.pack(fill=tk.X, pady=5)
        
        # 批量大小
        ttk.Label(params_frame, text="批量大小:").grid(row=0, column=0, sticky=tk.W, pady=2)
        self.batch_size_var = tk.IntVar(value=32)
        ttk.Entry(params_frame, textvariable=self.batch_size_var, width=10).grid(row=0, column=1, sticky=tk.W, pady=2)
        
        # Epochs
        ttk.Label(params_frame, text="Epochs:").grid(row=1, column=0, sticky=tk.W, pady=2)
        self.epochs_var = tk.IntVar(value=5)
        ttk.Entry(params_frame, textvariable=self.epochs_var, width=10).grid(row=1, column=1, sticky=tk.W, pady=2)
        
        # 学习率
        ttk.Label(params_frame, text="学习率:").grid(row=2, column=0, sticky=tk.W, pady=2)
        self.lr_var = tk.DoubleVar(value=0.001)
        ttk.Entry(params_frame, textvariable=self.lr_var, width=10).grid(row=2, column=1, sticky=tk.W, pady=2)
        
        # 训练和加载模型按钮
        btn_frame = ttk.Frame(train_frame)
        btn_frame.pack(fill=tk.X, pady=5)
        
        self.train_btn = ttk.Button(btn_frame, text="训练模型", command=self.train_model)
        self.train_btn.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=2)
        self.train_btn['state'] = 'disabled'
        
        self.save_btn = ttk.Button(btn_frame, text="保存模型", command=self.save_model)
        self.save_btn.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=2)
        self.save_btn['state'] = 'disabled'
        
        self.load_model_btn = ttk.Button(btn_frame, text="加载模型", command=self.load_saved_model)
        self.load_model_btn.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=2)
        
        # 进度条
        self.progress_var = tk.DoubleVar()
        self.progress_bar = ttk.Progressbar(train_frame, variable=self.progress_var, mode='determinate')
        self.progress_bar.pack(fill=tk.X, pady=5)
        
        # 状态标签
        self.status_var = tk.StringVar(value="准备就绪")
        ttk.Label(train_frame, textvariable=self.status_var).pack(anchor=tk.W, pady=5)
        
        # 准确率标签
        self.accuracy_var = tk.StringVar(value="测试准确率: N/A")
        ttk.Label(train_frame, textvariable=self.accuracy_var).pack(anchor=tk.W, pady=5)
        
        # 训练曲线
        self.fig, self.ax = plt.subplots(2, 1, figsize=(5, 6))
        self.canvas = FigureCanvasTkAgg(self.fig, master=train_frame)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        # 右侧 - 手写识别部分
        draw_frame = ttk.LabelFrame(right_frame, text="手写数字识别", padding=10)
        draw_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # 创建画布
        self.canvas_width = 280
        self.canvas_height = 280
        self.draw_canvas = tk.Canvas(draw_frame, width=self.canvas_width, height=self.canvas_height, 
                                    bg="black", highlightthickness=1, highlightbackground="gray")
        self.draw_canvas.pack(padx=5, pady=5)
        
        # 绑定鼠标事件
        self.image = PIL.Image.new("L", (self.canvas_width, self.canvas_height), "black")
        self.draw = ImageDraw.Draw(self.image)
        
        self.draw_canvas.bind("<B1-Motion>", self.paint)
        self.draw_canvas.bind("<ButtonRelease-1>", self.reset_position)
        self.prev_x = None
        self.prev_y = None
        
        # 按钮框架
        btn_frame2 = ttk.Frame(draw_frame)
        btn_frame2.pack(fill=tk.X, pady=5)
        
        self.clear_btn = ttk.Button(btn_frame2, text="清除", command=self.clear_canvas)
        self.clear_btn.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=2)
        
        self.predict_btn = ttk.Button(btn_frame2, text="识别", command=self.predict_digit)
        self.predict_btn.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=2)
        self.predict_btn['state'] = 'disabled'
        
        # 预测结果
        result_frame = ttk.LabelFrame(draw_frame, text="识别结果", padding=10)
        result_frame.pack(fill=tk.X, pady=10)
        
        self.result_var = tk.StringVar(value="请先绘制数字")
        self.result_label = ttk.Label(result_frame, textvariable=self.result_var, font=('Arial', 24))
        self.result_label.pack(pady=10)
        
        # 预测概率
        self.fig2, self.ax2 = plt.subplots(figsize=(5, 2))
        self.canvas2 = FigureCanvasTkAgg(self.fig2, master=result_frame)
        self.canvas2.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        # 模型架构说明
        model_info_frame = ttk.LabelFrame(right_frame, text="LeNet-5 模型架构", padding=10)
        model_info_frame.pack(fill=tk.X, pady=10)
        
        model_info = """
LeNet-5 架构:
- 输入层: 28x28x1
- 卷积层 C1: 6个5x5卷积核，得到24x24x6特征图
- 池化层 S2: 2x2最大池化，得到12x12x6特征图
- 卷积层 C3: 16个5x5卷积核，得到8x8x16特征图
- 池化层 S4: 2x2最大池化，得到4x4x16特征图
- 全连接层 F5: 120个神经元
- 全连接层 F6: 84个神经元
- 输出层: 10个神经元 (0-9数字)
        """
        ttk.Label(model_info_frame, text=model_info, justify=tk.LEFT).pack(anchor=tk.W)
    
    def load_data(self):
        self.status_var.set("正在加载数据...")
        self.root.update()
        
        # 定义转换
        transform = transforms.Compose([
            transforms.ToTensor(),
            transforms.Normalize((0.1307,), (0.3081,))
        ])
        
        # 加载MNIST数据集
        try:
            train_dataset = datasets.MNIST(root='./data', train=True, download=True, transform=transform)
            test_dataset = datasets.MNIST(root='./data', train=False, download=True, transform=transform)
            
            batch_size = self.batch_size_var.get()
            self.train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True)
            self.test_loader = DataLoader(test_dataset, batch_size=batch_size, shuffle=False)
            
            self.status_var.set(f"数据加载完成! 训练样本: {len(train_dataset)}, 测试样本: {len(test_dataset)}")
            self.train_btn['state'] = 'normal'
        except Exception as e:
            self.status_var.set(f"加载数据失败: {str(e)}")
            messagebox.showerror("错误", f"加载数据时出错: {str(e)}")
    
    def build_model(self):
        # 构建LeNet-5模型
        model = LeNet5()
        model = model.to(self.device)
        return model
    
    def train_model(self):
        if self.train_loader is None:
            messagebox.showerror("错误", "请先加载数据!")
            return
        
        self.status_var.set("正在构建模型...")
        self.root.update()
        
        # 构建LeNet-5模型
        self.model = self.build_model()
        
        epochs = self.epochs_var.get()
        lr = self.lr_var.get()
        
        # 定义损失函数和优化器
        criterion = nn.CrossEntropyLoss()
        optimizer = optim.Adam(self.model.parameters(), lr=lr)
        
        self.status_var.set("正在训练模型...")
        self.progress_var.set(0)
        self.root.update()
        
        # 训练模型
        self.history = {"train_loss": [], "train_acc": [], "val_loss": [], "val_acc": []}
        
        for epoch in range(epochs):
            # 训练模式
            self.model.train()
            running_loss = 0.0
            correct = 0
            total = 0
            
            for i, (inputs, labels) in enumerate(self.train_loader):
                inputs, labels = inputs.to(self.device), labels.to(self.device)
                
                # 清零梯度
                optimizer.zero_grad()
                
                # 前向传播
                outputs = self.model(inputs)
                loss = criterion(outputs, labels)
                
                # 反向传播和优化
                loss.backward()
                optimizer.step()
                
                # 统计
                running_loss += loss.item()
                _, predicted = torch.max(outputs.data, 1)
                total += labels.size(0)
                correct += (predicted == labels).sum().item()
                
                # 更新状态
                if i % 10 == 0:
                    self.status_var.set(f"训练中... Epoch {epoch+1}/{epochs}, Batch {i+1}/{len(self.train_loader)}")
                    self.root.update()
            
            # 计算训练集上的准确率
            train_loss = running_loss / len(self.train_loader)
            train_acc = 100 * correct / total
            
            # 验证模式
            self.model.eval()
            val_loss = 0.0
            correct = 0
            total = 0
            
            with torch.no_grad():
                for inputs, labels in self.test_loader:
                    inputs, labels = inputs.to(self.device), labels.to(self.device)
                    outputs = self.model(inputs)
                    loss = criterion(outputs, labels)
                    
                    val_loss += loss.item()
                    _, predicted = torch.max(outputs.data, 1)
                    total += labels.size(0)
                    correct += (predicted == labels).sum().item()
            
            # 计算验证集上的准确率
            val_loss = val_loss / len(self.test_loader)
            val_acc = 100 * correct / total
            
            # 保存历史
            self.history["train_loss"].append(train_loss)
            self.history["train_acc"].append(train_acc)
            self.history["val_loss"].append(val_loss)
            self.history["val_acc"].append(val_acc)
            
            # 更新进度和状态
            progress = (epoch + 1) / epochs * 100
            self.progress_var.set(progress)
            self.status_var.set(f"Epoch {epoch+1}/{epochs}, Loss: {train_loss:.4f}, Acc: {train_acc:.2f}%, Val Loss: {val_loss:.4f}, Val Acc: {val_acc:.2f}%")
            
            # 更新图表
            self.update_plots()
            self.root.update()
        
        # 评估模型
        self.model.eval()
        correct = 0
        total = 0
        
        with torch.no_grad():
            for inputs, labels in self.test_loader:
                inputs, labels = inputs.to(self.device), labels.to(self.device)
                outputs = self.model(inputs)
                _, predicted = torch.max(outputs.data, 1)
                total += labels.size(0)
                correct += (predicted == labels).sum().item()
        
        test_acc = 100 * correct / total
        self.accuracy_var.set(f"测试准确率: {test_acc:.2f}%")
        
        self.status_var.set("模型训练完成!")
        self.is_model_trained = True
        self.save_btn['state'] = 'normal'
        self.predict_btn['state'] = 'normal'
    
    def update_plots(self):
        # 清除旧图表
        self.ax[0].clear()
        self.ax[1].clear()
        
        epochs_range = range(1, len(self.history["train_loss"]) + 1)
        
        # 绘制损失曲线
        self.ax[0].plot(epochs_range, self.history["train_loss"], 'b', label='训练损失')
        self.ax[0].plot(epochs_range, self.history["val_loss"], 'r', label='验证损失')
        self.ax[0].set_title('训练和验证损失')
        self.ax[0].set_ylabel('损失')
        self.ax[0].legend()
        
        # 绘制准确率曲线
        self.ax[1].plot(epochs_range, self.history["train_acc"], 'b', label='训练准确率')
        self.ax[1].plot(epochs_range, self.history["val_acc"], 'r', label='验证准确率')
        self.ax[1].set_title('训练和验证准确率')
        self.ax[1].set_xlabel('Epoch')
        self.ax[1].set_ylabel('准确率')
        self.ax[1].legend()
        
        self.fig.tight_layout()
        self.canvas.draw()
    
    def save_model(self):
        if not self.is_model_trained or self.model is None:
            messagebox.showerror("错误", "没有可保存的模型!")
            return
        
        file_path = filedialog.asksaveasfilename(
            defaultextension=".pth",
            filetypes=[("PyTorch Model", "*.pth"), ("All files", "*.*")]
        )
        
        if file_path:
            torch.save(self.model.state_dict(), file_path)
            messagebox.showinfo("保存成功", f"模型已保存到: {file_path}")
    
    def load_saved_model(self):
        file_path = filedialog.askopenfilename(
            filetypes=[("PyTorch Model", "*.pth"), ("All files", "*.*")]
        )
        
        if file_path:
            try:
                self.model = self.build_model()
                self.model.load_state_dict(torch.load(file_path, map_location=self.device))
                self.model.eval()
                self.is_model_trained = True
                self.predict_btn['state'] = 'normal'
                self.status_var.set(f"模型已加载: {os.path.basename(file_path)}")
                messagebox.showinfo("加载成功", "模型加载成功!")
            except Exception as e:
                messagebox.showerror("错误", f"加载模型时出错: {str(e)}")
    
    def paint(self, event):
        x, y = event.x, event.y
        r = 15  # 画笔半径
        
        if self.prev_x is not None and self.prev_y is not None:
            self.draw_canvas.create_line(self.prev_x, self.prev_y, x, y, 
                                        width=r*2, fill="white", 
                                        capstyle=tk.ROUND, smooth=True)
            
            # 同时更新PIL图像
            self.draw.line([self.prev_x, self.prev_y, x, y], fill="white", width=r*2)
        else:
            # 绘制单个点
            self.draw_canvas.create_oval(x-r, y-r, x+r, y+r, fill="white", outline="white")
            self.draw.ellipse([x-r, y-r, x+r, y+r], fill="white", outline="white")
        
        self.prev_x = x
        self.prev_y = y
    
    def reset_position(self, event):
        self.prev_x = None
        self.prev_y = None
    
    def clear_canvas(self):
        self.draw_canvas.delete("all")
        self.image = PIL.Image.new("L", (self.canvas_width, self.canvas_height), "black")
        self.draw = ImageDraw.Draw(self.image)
        self.result_var.set("请先绘制数字")
        self.ax2.clear()
        self.canvas2.draw()
    
    def predict_digit(self):
        if not self.is_model_trained or self.model is None:
            messagebox.showerror("错误", "请先训练或加载模型!")
            return
        
        # 调整图像大小为28x28
        img = self.image.resize((28, 28), PIL.Image.LANCZOS)
        
        # 转换为PyTorch张量并进行预处理
        transform = transforms.Compose([
            transforms.ToTensor(),
            transforms.Normalize((0.1307,), (0.3081,))
        ])
        img_tensor = transform(img).unsqueeze(0).to(self.device)
        
        # 进行预测
        self.model.eval()
        with torch.no_grad():
            outputs = self.model(img_tensor)
            probabilities = F.softmax(outputs, dim=1)[0]
            digit = torch.argmax(probabilities).item()
            confidence = probabilities[digit].item() * 100
        
        # 显示结果
        self.result_var.set(f"预测结果: {digit} (置信度: {confidence:.2f}%)")
        
        # 更新条形图
        self.ax2.clear()
        indices = np.arange(10)
        self.ax2.bar(indices, probabilities.cpu().numpy())
        self.ax2.set_xticks(indices)
        self.ax2.set_xticklabels(indices)
        self.ax2.set_ylim([0, 1])
        self.ax2.set_title('预测概率')
        self.fig2.tight_layout()
        self.canvas2.draw()

if __name__ == "__main__":
    root = tk.Tk()
    app = DigitRecognitionApp(root)
    root.mainloop()
