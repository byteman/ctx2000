#! /bin/sh

dns1="211.95.193.97"
dns2="211.136.20.203"
dial_delay_s=20
is_pppd_exist()
{
	pidof pppd
	return $?
}
is_chat_exist()
{
	pidof chat
	return $?
}
cutoff_chat()
{
	killall chat
}
is_dial_ok()
{
	/sbin/ifconfig ppp0
	return $?
}
call_pppd_dial()
{
	/usr/sbin/pppd call gprs &
	
	while(is_dial_ok
	
}
dial_again()
{
      echo "dial_again"
      killall pppd        #����PPPD����
      /usr/sbin/pppd call gprs & #��ȥ����
}
trap 'dial_again' 50
sleep $dial_delay_s
dial_again
sleep 20
while true
do      
       echo "ping begin"
       ping -s 1 -c 1 $dns1    #ȥPING��һ��DNS!
       if [ "$?" != "0" ]      #����PING��ͨ
       then
           echo "ping failed1"
           ping -s 1 -c 2 $dns2   #ȥPING�ڶ���DNS
           if [ "$?" != "0" ]     #����PING��ͨ
           then   
              echo "ping failed2"
              killall pppd        #����PPPD����
              /usr/sbin/pppd call gprs & #��ȥ����
              sleep 20               #�ȴ�12��
           else
              echo "ping ok2"
              sleep 5            #�����PING  DNS2ͨ�Ļ���ֱ�ӵȴ�5��
           fi   
       else
              echo "sleep 20s"
              sleep 20           #�����PING  DNS1ͨ�Ļ���ֱ�ӵȴ�5�루һ��Ҫ���ö೤ʱ��ȥPING������
              echo "sleep 20s ok"

       fi
done
