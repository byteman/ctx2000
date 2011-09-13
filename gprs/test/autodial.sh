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
      killall pppd        #结束PPPD进程
      /usr/sbin/pppd call gprs & #再去拨号
}
trap 'dial_again' 50
sleep $dial_delay_s
dial_again
sleep 20
while true
do      
       echo "ping begin"
       ping -s 1 -c 1 $dns1    #去PING第一个DNS!
       if [ "$?" != "0" ]      #假如PING不通
       then
           echo "ping failed1"
           ping -s 1 -c 2 $dns2   #去PING第二个DNS
           if [ "$?" != "0" ]     #假如PING不通
           then   
              echo "ping failed2"
              killall pppd        #结束PPPD进程
              /usr/sbin/pppd call gprs & #再去拨号
              sleep 20               #等待12秒
           else
              echo "ping ok2"
              sleep 5            #如果是PING  DNS2通的话就直接等待5秒
           fi   
       else
              echo "sleep 20s"
              sleep 20           #如果是PING  DNS1通的话就直接等待5秒（一般要设置多长时间去PING请改这里）
              echo "sleep 20s ok"

       fi
done
