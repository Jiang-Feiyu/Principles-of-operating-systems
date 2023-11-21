grep "Failed" auth.log | grep -oE "from ([0-9]{1,3}\.){3}[0-9]{1,3}" | awk '{print $2}' > suspicious_ips.txt
