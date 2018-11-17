#coding:utf-8
import socket
import threading
import ctypes
gDict={}#保存客户的sock

MySqlDll = ctypes.windll.LoadLibrary("..\\..\\MySqlDll.dll")
HandleSql = MySqlDll.MySql_LineMySql("127.0.0.1".encode("gbk"), "root".encode("gbk"), "admin".encode("gbk"),"mychat".encode("gbk"), 3306)
if(HandleSql == 0):
    print("数据库连接失败！" )
print("数据库连接成功！")
gSocketSever = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
gSocketSever.bind(("127.0.0.1", 9999))
gSocketSever.listen(5)
print("服务端启动成功！")

def main():
    MySqlDll.MySql_Execute(HandleSql, "SET NAMES GBK".encode("gbk"))
    MySqlDll.MySql_Execute(HandleSql, "UPDATE info SET sock='-1',clock='0' where sock !='-1' or clock!='0'".encode("gbk"))
    print("重置数据库成功！")
    nT = threading.Thread(target=AcceptProc)
    nT.start()
def AcceptProc():
    while True:
        nSocketClient,nAddrClient = gSocketSever.accept()
        print(nSocketClient,"连接成功！")
        nT = threading.Thread(target=RecvDataProc,args=(nSocketClient,nAddrClient))
        nT.start()
def RecvDataProc(*arg):
    nSocketClient=arg[0]
    nAddrClient=arg[1]
    while True:
        try:
            nRecvByte =  nSocketClient.recv(65535)
            nRecvByte=bytearray(nRecvByte)
            nRecvTemp =bytearray(nRecvByte)

            for x in range(len(nRecvByte)):
                nRecvByte[x]^=5

            nMessage = nRecvByte.decode("utf-16")

            if(nMessage==""):
                print(nAddrClient,"客户端退出！")
                try:
                    del gDict[nAddrClient]
                    DeleteSock(nAddrClient)
                    nSocketClient.close()
                except:
                    print("处理删除sock异常")
                return

            print(nAddrClient, " - 解密前：", nRecvTemp.decode("utf-16")," - 解密后：" ,nMessage)

            nList = nMessage.split("|#|")
            HeadData=nList[0]
            if(HeadData == "登录"):
                nId = nList[1]
                nPassword = nList[2]

                nRet= IsIdAndPassTrue(nId,nPassword)
                if(nRet[0]==1):
                    nSocketClient.send(str("登录成功|#|" + nRet[1] +"|#|"+ nRet[2]+"|#|"+ nRet[3]).encode("gbk"))
                    gDict[nAddrClient]=nSocketClient
                    UpdateSock(nId,nAddrClient)
                elif(nRet[0]==-1):
                    nSocketClient.send("帐号已登录|#|0|#|0|#|0".encode("gbk"))
                elif (nRet[0] == 0):
                    nSocketClient.send("帐号密码错误|#|0|#|0|#|0".encode("gbk"))

            elif(HeadData == "注册"):
                nId = nList[1]
                nPassword = nList[2]
                nName = nList[3]
                nSex = nList[4]
                nPic = nList[5]
                if(AddAccount(nId,nPassword,nName,nSex,nPic)):
                    print("注册成功！",nId,nPassword,nName)
                    nSocketClient.send("注册成功|#|0".encode("gbk"))
                else:
                    print("帐号存在！", nId, nPassword, nName)
                    nSocketClient.send("帐号存在|#|0".encode("gbk"))

            elif (HeadData == "聊天室"):
                TransRoomMsg(nList[1],nList[2],nList[3],nList[4],nList[5])
                AddRoomRecord(nList[1],nList[3],nList[4],nList[5])

            elif (HeadData == "私聊"):
                nSock = FindSock(nList[1])
                if(nSock=="-1"):
                    nSocketClient.send(str("私聊|#|"+nList[2]+"|#|"+nList[1]+"|#|系统消息|#|"+nList[4]+"|#|对方目前不在线！").encode("gbk"))
                else:
                    nTempSocketClient = StrToSockTuple(nSock)
                    gDict[nTempSocketClient].send(nMessage.encode("gbk"))
                    AddSingleRecord(nList[1],nList[2],nList[3],nList[4],nList[5])

            elif (HeadData == "发送文件" or HeadData == "拒绝文件" or HeadData == "接受文件" or HeadData == "准备发送"):
                nSock = FindSock(nList[1])
                if(nSock=="-1"):
                    nSocketClient.send(str("私聊|#|"+nList[2]+"|#|"+nList[1]+"|#|系统消息|#| |#|对方目前不在线！无法发送文件！").encode("gbk"))
                else:
                    nTempSocketClient = StrToSockTuple(nSock)
                    if(HeadData == "准备发送"):
                        nMessage=str(nMessage+"|#|"+nAddrClient[0])#补上服务端获取的IP
                    gDict[nTempSocketClient].send(nMessage.encode("gbk"))

            elif (HeadData == "视频聊天" or HeadData == "接受视频" or HeadData == "拒绝视频" or HeadData == "关闭视频"):
                nSock = FindSock(nList[1])
                if(nSock=="-1"):
                    nSocketClient.send(str("私聊|#|"+nList[2]+"|#|"+nList[1]+"|#|系统消息|#| |#|对方目前不在线！无法进行视频聊天！").encode("gbk"))
                else:
                    nTempSocketClient = StrToSockTuple(nSock)
                    nMessage=str(nMessage+"|#|"+nAddrClient[0])#补上服务端获取的IP
                    gDict[nTempSocketClient].send(nMessage.encode("gbk"))


            elif (HeadData == "心跳包"):
                nSocketClient.send(str("列表信息|#|" + GetFriend(nList[1])+"|#|"+GetRoom(nList[1])).encode("gbk"))

            elif (HeadData == "聊天室成员"):
                nSocketClient.send(str("聊天室成员|#|" + nList[1]+"|#|"+GetRoomPeople(nList[1])).encode("gbk"))

            elif (HeadData == "查找好友"):
                nSocketClient.send(str("查找好友|#|" + FindFriend(nList[1])).encode("gbk"))

            elif (HeadData == "请求添加好友"):
                nSock = FindSock(nList[1])
                nTempSocketClient = StrToSockTuple(nSock)
                gDict[nTempSocketClient].send(str("请求添加好友|#|" + nList[2]+"|#|"+nList[3]).encode("gbk"))

            elif (HeadData == "同意添加好友"):
                nSock = FindSock(nList[1])
                nTempSocketClient = StrToSockTuple(nSock)
                gDict[nTempSocketClient].send(str("同意添加好友|#|" + nList[2]+"|#|"+nList[3]).encode("gbk"))
                AddFriendRelation(nList[1],nList[2])

            elif (HeadData == "拒绝添加好友"):
                nSock = FindSock(nList[1])
                nTempSocketClient = StrToSockTuple(nSock)
                gDict[nTempSocketClient].send(str("拒绝添加好友|#|" + nList[2]+"|#|"+nList[3]).encode("gbk"))

            elif (HeadData == "删除好友"):
                DeleteFriendRelation(nList[1],nList[2])

            elif (HeadData == "查看私聊记录"):
                nSocketClient.send(str("查看私聊记录|#|" +nList[1]+ "|#|"+GetSingleRecord(nList[2],nList[1])).encode("gbk"))

            elif (HeadData == "查看聊天室记录"):
                nSocketClient.send(str("查看聊天室记录|#|" +nList[1]+ "|#|"+GetRoomRecord(nList[1])).encode("gbk"))

            elif (HeadData == "退出聊天室"):
                nRoomId = nList[1]
                nId = nList[2]
                OutRoom(nRoomId,nId)
                #TransRoomMsg(nRoomId,"","系统消息：",GetRoomName(nRoomId),GetUserName(nId)+" 退出聊天室！")
                TransRoomMsg(nRoomId,"","系统消息："," ",GetUserName(nId)+" 退出聊天室！")

            elif (HeadData == "加入聊天室"):
                nRoomId=nList[1]
                nId=nList[2]
                ComeRoom(nRoomId,nId)
                #TransRoomMsg(nRoomId,"","系统消息：",GetRoomName(nRoomId),"欢迎 "+GetUserName(nId)+" 加入聊天室！")
                TransRoomMsg(nRoomId,"","系统消息："," ","欢迎 "+GetUserName(nId)+" 加入聊天室！")


            elif (HeadData == "创建聊天室"):
                if(CreatRoom(nList[1],nList[2])):
                    nSocketClient.send(str("创建聊天室|#|创建成功").encode("gbk"))
                    ComeRoom(nList[1], nList[3])
                else:
                    nSocketClient.send(str("创建聊天室|#|创建失败").encode("gbk"))

            elif (HeadData == "查找聊天室"):
                nSocketClient.send(str("查找聊天室|#|"+FindRoom(nList[1])).encode("gbk"))

            elif (HeadData == "修改资料"):
                ChangeInfo(nList[1],nList[2],nList[3],nList[4])

        except:
            print("异常！")
            return
def ChangeInfo(nId,nPic,nName,nSex):
    nStr = "update info set pic='"+nPic+"',name='"+nName+"',sex='"+nSex+"' where id='" + nId+"'"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
def GetUserName(nId):
    nStr = "select * from info where id ='"+nId+"'"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nVal =  MySqlDll.MySql_GetField(nRecord,"name".encode("gbk"))
    if(nVal==0):return ("NULL")
    nTempName = ctypes.string_at(nVal, -1).decode('gbk')
    return nTempName
def GetRoomName(nRoomId):
    nStr = "select * from roomlist where roomid ='"+nRoomId+"'"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nVal =  MySqlDll.MySql_GetField(nRecord,"roomname".encode("gbk"))
    if(nVal==0):return ("NULL")
    nTempName = ctypes.string_at(nVal, -1).decode('gbk')
    return nTempName
def IsIdAndPassTrue(nId,nPassWord):

    nStr = "select * from info where id ='"+nId+"' and password = md5("+nPassWord+")"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))

    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nVal =  MySqlDll.MySql_GetField(nRecord,"name".encode("gbk"))
    if(nVal==0):return (0,"0","0","0")
    nTempName = ctypes.string_at(nVal, -1).decode('gbk')

    nVal =  MySqlDll.MySql_GetField(nRecord,"password".encode("gbk"))
    nTempPassWord = ctypes.string_at(nVal, -1).decode('gbk')

    nVal =  MySqlDll.MySql_GetField(nRecord,"sock".encode("gbk"))
    nTempSock = ctypes.string_at(nVal, -1).decode('gbk')

    nVal =  MySqlDll.MySql_GetField(nRecord,"pic".encode("gbk"))
    nTempPic = ctypes.string_at(nVal, -1).decode('gbk')

    nVal =  MySqlDll.MySql_GetField(nRecord,"sex".encode("gbk"))
    nTempSex = ctypes.string_at(nVal, -1).decode('gbk')

    MySqlDll.MySql_FreeRecord(nRecord)

    if(nTempSock!="-1"):
        return (-1,nTempName,"0","0")
    return (1,nTempName,nTempPic,nTempSex)
def AddAccount(nId,nPassWord,nName,nSex,nPic):
    nStr = "select * from info where id = '" + nId + "'"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nVal =  MySqlDll.MySql_GetField(nRecord,"id".encode("gbk"))
    if(nVal!=0):return False
    MySqlDll.MySql_FreeRecord(nRecord)

    nStr = "insert into info (`id`, `password`, `name`, `sock`, `clock`,`sex`,`pic`) values ('"+nId+"' ,md5("+nPassWord+"),'"+nName+"','-1','0','"+nSex+"','"+nPic+"')"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))

    nStr = "create table singlerecord_"+nId+" (recordid int AUTO_INCREMENT PRIMARY KEY, friendid char(32), recordname varchar(255),recordtime varchar(255),recorddata varchar(255))"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
    return True
def TransRoomMsg(nRoomId,nRoomName,nSendName,nSendTime,nSendData):
    nStr = "select * from info,room where info.sock!=-1 and info.id = room.id and room.roomid='" + nRoomId + "'"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nRecordNum = MySqlDll.MySql_GetRecordNum(nRecord)
    for x in range(nRecordNum):
        nVal = MySqlDll.MySql_GetField(nRecord, "sock".encode("gbk"))
        nTempSock = ctypes.string_at(nVal, -1).decode('gbk')

        nVal = MySqlDll.MySql_GetField(nRecord, "id".encode("gbk"))
        nTempId = ctypes.string_at(nVal, -1).decode('gbk')

        nTempAddrClient=StrToSockTuple(nTempSock)

        gDict[nTempAddrClient].send(str("聊天室|#|"+nRoomId+"|#|"+nRoomName+"|#|"+nSendName+"|#|"+nSendTime+"|#|"+nSendData).encode("gbk"))
        MySqlDll.MySql_NextRecord(nRecord)
    MySqlDll.MySql_FreeRecord(nRecord)
def AddRoomRecord(nRoomId,nSendName,nSendTime,nSendData):
    nStr = "INSERT INTO roomrecord_"+nRoomId+" (`recordname`, `recordtime`, `recorddata`) VALUES ('"+nSendName+"','"+nSendTime+"','"+nSendData+"')"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
def AddSingleRecord(nId1,nId2,nSendName,nSendTime,nSendData):
    nStr = "INSERT INTO singlerecord_"+nId1+" (`friendid`, `recordname`, `recordtime`, `recorddata`) VALUES ('"+nId2+"' ,'"+nSendName+"','"+nSendTime+"','"+nSendData+"')"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))

    nStr = "INSERT INTO singlerecord_"+nId2+" (`friendid`, `recordname`, `recordtime`, `recorddata`) VALUES ('"+nId1+"' ,'"+nSendName+"','"+nSendTime+"','"+nSendData+"')"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
def UpdateSock(nId,nSock):
    nTempSock = nSock[0] + ":" + str(nSock[1])
    nStr = "update info set sock='"+nTempSock+"', clock='10' where id='"+nId+"'"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
def DeleteSock(nSock):
    nTempSock = nSock[0] + ":" + str(nSock[1])
    nStr = "update info set sock='-1',clock='0' where sock='" + nTempSock+"'"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
def FindSock(nId):
    nStr = "select * from info where id = '"+nId+"'"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nVal = MySqlDll.MySql_GetField(nRecord, "sock".encode("gbk"))
    nTempSock = ctypes.string_at(nVal, -1).decode('gbk')
    MySqlDll.MySql_FreeRecord(nRecord)
    return nTempSock
def StrToSockTuple(nSockStr):
    nList = nSockStr.split(":")
    nIp = str(nList[0])
    nPort = int(nList[1])
    nTempAddrClient = (nIp, nPort)
    return nTempAddrClient
def GetFriend(nId):
    nStr = "select * from info,friend where friend.myid = '"+nId+"' and info.id=friend.friendid"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nRecordNum = MySqlDll.MySql_GetRecordNum(nRecord)

    nRetOnlineList=""

    for x in range(nRecordNum):
        nVal = MySqlDll.MySql_GetField(nRecord, "sock".encode("gbk"))
        nTempSock = ctypes.string_at(nVal, -1).decode('gbk')
        nVal = MySqlDll.MySql_GetField(nRecord, "name".encode("gbk"))
        nTempName = ctypes.string_at(nVal, -1).decode('gbk')
        nVal = MySqlDll.MySql_GetField(nRecord, "friendid".encode("gbk"))
        nTempFriendid = ctypes.string_at(nVal, -1).decode('gbk')
        nVal = MySqlDll.MySql_GetField(nRecord, "pic".encode("gbk"))
        nTempPic = ctypes.string_at(nVal, -1).decode('gbk')
        nOnline=""
        if(nTempSock=="-1"):
            nOnline = "离线"
        else:
            nOnline = "在线"

        if(x==nRecordNum-1):
            nRetOnlineList = nRetOnlineList + nTempFriendid + "#" + nTempName + "#" + nOnline + "#" + nTempPic
        else:
            nRetOnlineList = nRetOnlineList + nTempFriendid + "#" + nTempName + "#" + nOnline + "#" + nTempPic + "|||"
        MySqlDll.MySql_NextRecord(nRecord)

    MySqlDll.MySql_FreeRecord(nRecord)
    return nRetOnlineList
def GetRoomPeople(nRoomId):
    nStr = "select * from room,info where room.roomid = '"+nRoomId+"' and room.id = info.id"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nRecordNum = MySqlDll.MySql_GetRecordNum(nRecord)

    nRetOnlineList=""

    for x in range(nRecordNum):
        nVal = MySqlDll.MySql_GetField(nRecord, "id".encode("gbk"))
        nTempId = ctypes.string_at(nVal, -1).decode('gbk')
        nVal = MySqlDll.MySql_GetField(nRecord, "name".encode("gbk"))
        nTempName = ctypes.string_at(nVal, -1).decode('gbk')
        nVal = MySqlDll.MySql_GetField(nRecord, "sock".encode("gbk"))
        nTempSock = ctypes.string_at(nVal, -1).decode('gbk')
        nOnline=""
        if(nTempSock=="-1"):
            nOnline = "离线"
        else:
            nOnline = "在线"

        if(x==nRecordNum-1):
            nRetOnlineList = nRetOnlineList + nTempId + "#" + nTempName + "#" + nOnline
        else:
            nRetOnlineList = nRetOnlineList + nTempId + "#" + nTempName + "#" + nOnline + "|||"
        MySqlDll.MySql_NextRecord(nRecord)

    MySqlDll.MySql_FreeRecord(nRecord)
    return nRetOnlineList
def GetRoom(nId):
    nStr = "select * from room,roomlist where room.id = '"+nId+"' and room.roomid=roomlist.roomid"
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nRecordNum = MySqlDll.MySql_GetRecordNum(nRecord)

    nRetRoomList = ""

    for x in range(nRecordNum):
        nVal = MySqlDll.MySql_GetField(nRecord, "roomid".encode("gbk"))
        nTempRoomId = ctypes.string_at(nVal, -1).decode('gbk')
        nVal = MySqlDll.MySql_GetField(nRecord, "roomname".encode("gbk"))
        nTempRoomName = ctypes.string_at(nVal, -1).decode('gbk')

        if (x == nRecordNum - 1):
            nRetRoomList = nRetRoomList + nTempRoomId + "#" + nTempRoomName
        else:
            nRetRoomList = nRetRoomList + nTempRoomId + "#" + nTempRoomName + "|||"
        MySqlDll.MySql_NextRecord(nRecord)

    MySqlDll.MySql_FreeRecord(nRecord)
    return nRetRoomList
def FindFriend(nId):
    nStr = "select * from info where id="+nId
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)

    nVal = MySqlDll.MySql_GetField(nRecord, "name".encode("gbk"))
    if(nVal==0):return "不存在|||不存在"
    nTempName = ctypes.string_at(nVal, -1).decode('gbk')

    nVal = MySqlDll.MySql_GetField(nRecord, "sock".encode("gbk"))
    nTempSock = ctypes.string_at(nVal, -1).decode('gbk')

    MySqlDll.MySql_FreeRecord(nRecord)

    if(nTempSock=="-1"):return "不在线|||"+nId+"|||"+nTempName
    return nId+"|||"+nTempName
def AddFriendRelation(nId1,nId2):
    nStr = "insert into friend (`myid`, `friendid`) values ('"+nId1+"' ,'"+nId2+"')"
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
    nStr = "insert into friend (`myid`, `friendid`) values ('"+nId2+"' ,'"+nId1+"')"
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
def DeleteFriendRelation(nId1,nId2):
    nStr = "delete from friend where myid ='"+nId1+"' and friendid = '"+nId2+"'"
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
    nStr = "delete from friend where myid ='"+nId2+"' and friendid = '"+nId1+"'"
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
def GetSingleRecord(nId1,nId2):
    nStr = "select * from singlerecord_"+nId1+" where friendid = '"+nId2+"'"
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nRecordNum = MySqlDll.MySql_GetRecordNum(nRecord)

    nRetRecordList = ""

    for x in range(nRecordNum):
        nVal = MySqlDll.MySql_GetField(nRecord, "recordname".encode("gbk"))
        nTempName = ctypes.string_at(nVal, -1).decode('gbk')
        nVal = MySqlDll.MySql_GetField(nRecord, "recordtime".encode("gbk"))
        nTempTime = ctypes.string_at(nVal, -1).decode('gbk')
        nVal = MySqlDll.MySql_GetField(nRecord, "recorddata".encode("gbk"))
        nTempData = ctypes.string_at(nVal, -1).decode('gbk')

        if (x == nRecordNum - 1):
            nRetRecordList = nRetRecordList + nTempName + "|||" + nTempTime+ "|||" + nTempData
        else:
            nRetRecordList = nRetRecordList + nTempName + "|||" + nTempTime+ "|||" + nTempData + "|*|"
        MySqlDll.MySql_NextRecord(nRecord)

    MySqlDll.MySql_FreeRecord(nRecord)
    return nRetRecordList
def GetRoomRecord(nRoomId):
    nStr = "select * from roomrecord_"+nRoomId
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nRecordNum = MySqlDll.MySql_GetRecordNum(nRecord)

    nRetRecordList = ""

    for x in range(nRecordNum):
        nVal = MySqlDll.MySql_GetField(nRecord, "recordname".encode("gbk"))
        nTempName = ctypes.string_at(nVal, -1).decode('gbk')
        nVal = MySqlDll.MySql_GetField(nRecord, "recordtime".encode("gbk"))
        nTempTime = ctypes.string_at(nVal, -1).decode('gbk')
        nVal = MySqlDll.MySql_GetField(nRecord, "recorddata".encode("gbk"))
        nTempData = ctypes.string_at(nVal, -1).decode('gbk')

        if (x == nRecordNum - 1):
            nRetRecordList = nRetRecordList + nTempName + "|||" + nTempTime+ "|||" + nTempData
        else:
            nRetRecordList = nRetRecordList + nTempName + "|||" + nTempTime+ "|||" + nTempData + "|*|"
        MySqlDll.MySql_NextRecord(nRecord)

    MySqlDll.MySql_FreeRecord(nRecord)
    return nRetRecordList
def OutRoom(nRoomId,nId):
    nStr = "DELETE FROM room WHERE id ='"+nId+"' and roomid='"+nRoomId+"'"
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
def ComeRoom(nRoomId,nId):
    nStr = "INSERT INTO room (`id`, `roomid`) VALUES ('"+nId+"' ,'"+nRoomId+"')"
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
def CreatRoom(nRoomId,nRoomName):
    nStr = "select * from roomlist where roomid="+nRoomId
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nVal = MySqlDll.MySql_GetField(nRecord, "roomname".encode("gbk"))
    if(nVal!=0):return False
    #nTempRoomName = ctypes.string_at(nVal, -1).decode('gbk')
    MySqlDll.MySql_FreeRecord(nRecord)

    nStr = "INSERT INTO roomlist (`roomid`, `roomname`) VALUES ('"+nRoomId+"' ,'"+nRoomName+"')"
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))

    nStr = "create table roomrecord_"+nRoomId+" (recordid int AUTO_INCREMENT PRIMARY KEY, recordname varchar(255),recordtime varchar(255),recorddata varchar(255))"
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
    return True
def FindRoom(nRoomId):
    nStr = "select * from roomlist where roomid="+nRoomId
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nVal = MySqlDll.MySql_GetField(nRecord, "roomname".encode("gbk"))
    MySqlDll.MySql_FreeRecord(nRecord)
    if(nVal==0):return "不存在|||不存在|||0"
    nTempRoomName = ctypes.string_at(nVal, -1).decode('gbk')

    nStr = "select * from room where roomid="+nRoomId
    MySqlDll.MySql_Execute(HandleSql, nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nRecordNum = MySqlDll.MySql_GetRecordNum(nRecord)
    MySqlDll.MySql_FreeRecord(nRecord)
    return nRoomId+"|||"+nTempRoomName+"|||"+str(nRecordNum)

if __name__ == "__main__":
    main()

