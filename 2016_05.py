import Crypto.Hash.MD5;

input=b'cxdnnyjw'
PART2=True

i=0;
password=''
if PART2:
    password = ['\0','\0','\0','\0','\0','\0','\0','\0']

done = False;

while not done:
    thishash=""
    while not thishash.startswith("00000"):
        h = Crypto.Hash.MD5.new(input + bytes(str(i), encoding='utf-8'));
        thishash = h.hexdigest();
        i += 1
        #if 0 == i % 100000:
        #    print("  {}: {}".format(
        #        str(input + bytes(str(i), encoding='utf-8'), encoding='utf-8'),
        #        thishash));
    print("  {}: {}  (password so far: {})".format(
        str(input + bytes(str(i), encoding='utf-8'), encoding='utf-8'),
        thishash), ''.join(password));
    if PART2:
        pos = int(thishash[5], 16);
        if pos < 8 and '\0' == password[pos]:
            password[pos] = thishash[6];
        if not '\0' in password:
            done = True;
    else:
        password += thishash[5];
        done = (len(password) < 8);

print(''.join(password));

