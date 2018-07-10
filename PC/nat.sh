ssh root@10.10.10.1 "./fw.restart $1 $2"      
ifconfig enp0s3 down                          
ifconfig enp0s3 up                            
ip route add default via 10.10.10.1 dev enp0s3
