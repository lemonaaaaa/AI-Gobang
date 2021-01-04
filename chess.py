import pygame
import time
import sys
import os
from pygame.locals import *

initChessList = []		  	#保存棋盘坐标
initRole = 2				#游戏角色标志，1：白棋;2：黑棋
resultFlag = 0			  	#游戏结束标志，1：白棋胜；2：黑棋胜
chessSize = 9				#设置游戏棋盘大小
ban = False					#禁手标志

class ChessPoint():			#棋盘坐标及种类类
	def __init__(self,x,y,value):
		self.x = x						#x坐标
		self.y = y 						#y坐标
		self.value = value				#当前坐标点的棋子：0:没有棋子 1:白子 2:黑子

def initChessSquare(x,y):	 					#初始化棋盘
	for i in range(chessSize):	   				#每一行的交叉点坐标
		rowlist = []
		for j in range(chessSize):  			#每一列的交叉点坐标
			pointX = x + j*40					#得出在界面上的坐标
			pointY = y + i*40
			sp = ChessPoint(pointX,pointY,0)
			rowlist.append(sp)
		initChessList.append(rowlist)			#本质上是创建一个二维的数据结构，用于表示棋盘

def HumanGo():									#玩家执子走棋
	global initRole, initChessList, ban
	dropped = False								#玩家成功下子
	while True:
		UpdateChessMap()						#不断刷新界面以防卡顿
		for event in pygame.event.get():
			if event.type == QUIT:				#事件类型为退出时退出整个程序
				pygame.quit()
				sys.exit()
			if event.type == MOUSEBUTTONDOWN: 	#检测到点击鼠标
				x,y = pygame.mouse.get_pos()  	#获取点击鼠标的位置坐标
				i = 0
				j = 0
				for temp in initChessList:
					for point in temp:
						if x>=point.x-10 and x<=point.x+10 and y>=point.y-10 and y<=point.y+10:	#点击点合法
							if point.value == 0 and initRole == 1:   			#当棋盘位置为空；棋子类型为白棋
								chessfile = open(r'chessmap.txt','w')			#打开棋盘和落子的输出文件
								chessfile.write(str((point.x-27)/40))			#计算并输出即将落子的点的坐标
								chessfile.write(str((point.y-27)/40))
								for i in range(chessSize):
									for j in range(chessSize):
										chessfile.write(str(initChessList[i][j].value))
										chessfile.write(' ')					#输出棋盘状态
									chessfile.write('\n')
								chessfile.close()								#关闭文件
								os.system('ban.exe')							#运行禁手检测程序
								banfile = open(r'ban.txt','r')
								isBan = int(banfile.read())						#读入禁手检测结果，1为触犯禁手，0为不触犯禁手
								banfile.close()
								if(isBan == 1): ban = True						#触犯禁手，标记一下
								point.value = 1			 						#轮到白棋下
								judgeResult(i,j,1)								#判断是否产生胜局
								initRole = 2									#切换角色
								dropped = True									#成功落子
							elif point.value == 0 and initRole ==2:
								chessfile = open(r'chessmap.txt','w')
								chessfile.write(str((point.x-27)/40))
								chessfile.write(str((point.y-27)/40))
								for i in range(chessSize):
									for j in range(chessSize):
										chessfile.write(str(initChessList[i][j].value))
										chessfile.write(' ')
									chessfile.write('\n')
								chessfile.close()
								os.system('ban.exe')
								banfile = open(r'ban.txt','r')
								isBan = int(banfile.read())
								banfile.close()
								if(isBan == 1): ban=True
								point.value = 2
								judgeResult(i,j,2)
								initRole = 1
								dropped = True
							if(dropped == True): break
						j+=1
					i+=1
					j=0
		if(dropped == True): break				#已经下了棋，退出该函数
	

def AIGo():
	global initRole, initChessList
	chessfile = open(r'chessmap.txt','w')				#将棋盘输出到文件，以便于进一步地处理
	for i in range(chessSize):
		for j in range(chessSize):
			chessfile.write(str(initChessList[i][j].value))
			chessfile.write(' ')
		chessfile.write('\n')
	chessfile.close()
	print("Waiting 10 seconds for AI to think...")
	os.system('ai.exe')									#运行计算程序
	# rs = os.popen(r'once.exe')
	# time.sleep(15)
	#for tt in range(0,12):
	#	time.sleep(1)
	print('AI is Dropping chess...')
	posfile = open(r'chesspos.txt','r')
	List = posfile.read().split(',')
	posfile.close()
	y = int(List[0])									#得到x坐标
	x = int(List[1])									#得到y坐标
	print(x,y)											#调试信息，显示在控制台
	x = x*40+27											#计算出图形界面上的x坐标
	y = y*40+27											#计算出图形界面上的y坐标
	i = 0
	j = 0
	for temp in initChessList:
		for point in temp:
			if x>=point.x-10 and x<=point.x+10 and y>=point.y-10 and y<=point.y+10:
				if point.value == 0 and initRole == 1:   #当棋盘位置为空；棋子类型为白棋
					point.value = 1			 #鼠标点击时，棋子为白棋
					judgeResult(i,j,1)
					initRole = 2				#切换角色
				elif point.value == 0 and initRole ==2:  #当棋盘位置为空；棋子类型为黑棋
					point.value = 2			 #鼠标点击时，棋子为黑棋
					judgeResult(i,j,2)
					initRole = 1				#切换角色
				break
			j+=1
		i+=1
		j=0

def judgeResult(i,j,value):   #判断是否出现了五连
	global resultFlag
	flag = False
	#判断横向有没有出现五连
	for  x in  range(j - 4, j + 5):
		if x >= 0 and x + 4 < chessSize :
			if initChessList[i][x].value == value and \
				initChessList[i][x + 1].value == value and \
				initChessList[i][x + 2].value == value and \
				initChessList[i][x + 3].value == value and \
				initChessList[i][x + 4].value == value :
				flag = True
				break
				pass
	#判断纵向有没有出现五连
	for x in range(i - 4, i + 5):
		if x >= 0 and x + 4 < chessSize:
			if initChessList[x][j].value == value and \
					initChessList[x + 1][j].value == value and \
					initChessList[x + 2][j].value == value and \
					initChessList[x + 3][j].value == value and \
					initChessList[x + 4][j].value == value:
				flag = True
				break
				pass

	#判断反对角线上有没有出现五连
	for x, y in zip(range(j + 4, j - 5, -1), range(i - 4, i + 5)):
		if x >= 0 and x + 4 < chessSize and y + 4 >= 0 and y < chessSize:
			if initChessList[y][x].value == value and \
					initChessList[y - 1][x + 1].value == value and \
					initChessList[y - 2][x + 2].value == value and \
					initChessList[y - 3][x + 3].value == value and \
					initChessList[y - 4][x + 4].value == value:
				flag = True

	#判断对角线上有没有出现五连
	for x, y in zip(range(j - 4, j + 5), range(i - 4, i + 5)):
		if x >= 0 and x + 4 < chessSize and y >= 0 and y + 4 < chessSize:
			if initChessList[y][x].value == value and \
					initChessList[y + 1][x + 1].value == value and \
					initChessList[y + 2][x + 2].value == value and \
					initChessList[y + 3][x + 3].value == value and \
					initChessList[y + 4][x + 4].value == value:
				flag = True


	if flag:			   #如果条件成立，证明五子连珠
		resultFlag = value #获取成立的棋子颜色
		print("白棋赢" if value ==1 else "黑棋赢")

def UpdateChessMap():
	global initChessList, resultFlag, ban
	screen.blit(background,(0,0))			#绘制空背景
	for temp in initChessList:				#枚举遍历整个棋盘
		for point in temp:
			if point.value == 1:		  	#当棋子类型为1时，绘制白棋
				screen.blit(whiteChess,(point.x-18,point.y-18))
			elif point.value == 2:			#当棋子类型为2时，绘制黑棋
				screen.blit(blackChess,(point.x-18,point.y-18))

	if(ban == 1):
		initChessList = []				 	#清空棋盘
		initChessSquare(27,27)			 	#重新初始化棋盘
		screen.blit(banImg,(50,50))			#绘制禁手提示图片

	elif(resultFlag > 0):					#已经决出胜负
		initChessList = []					#清空棋盘
		initChessSquare(27,27)			 	#重新初始化棋盘
		if(resultFlag == 2):				#黑方胜利
			screen.blit(blackwin,(50,50)) 	#绘制黑方获胜时的图片
		else:								#白方胜利
			screen.blit(whitewin,(50,50)) 	#绘制白方获胜时的图片

	pygame.display.update()					#将所有的更新应用，绘制新的图像

	if(resultFlag > 0 or ban == 1):		#已经决出胜负或者触犯禁手
		time.sleep(3)					#显示图片等待时间
		resultFlag = 0					#重置游戏
		ban = 0							#重置游戏
		return True						#游戏结束

	return False						#游戏继续

def main():
	global initChessList, resultFlag, screen, background, whiteChess, blackChess, banImg, blackwin, whitewin
	initChessSquare(27,27)			#初始化棋盘的坐标，求出在界面上的坐标值
	pygame.init()	 				#初始化游戏环境
	screen = pygame.display.set_mode((chessSize*40+14,chessSize*40+14),0,0)		#创建游戏窗口
	pygame.display.set_caption("AI五子棋")					#设置游戏标题
	background = pygame.image.load("images/bg9.png")		#加载棋盘背景图片
	whiteChess = pygame.image.load("images/white.png") 		#加载白棋图片
	blackChess = pygame.image.load("images/black.png") 		#加载黑棋图片
	blackwin = pygame.image.load("images/blackwin.png")		#加载黑棋胜利图片
	whitewin = pygame.image.load("images/whitewin.png")		#加载白棋胜利图片
	banImg = pygame.image.load("images/ban.png")			#加载禁手提示图片	
	UpdateChessMap()							#绘制初始棋盘
	while True:									#玩家执黑先行
		HumanGo()								#玩家走子
		if(UpdateChessMap()==True): break		#玩家胜利并且已经完成了胜利界面的输出
		AIGo()									#电脑走子
		if(UpdateChessMap()==True): break		#电脑胜利并且已经完成了胜利界面的输出
	pass

if __name__ == '__main__':
	main()									#程序入口
	pass
