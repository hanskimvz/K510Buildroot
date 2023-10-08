You're looking for the chpasswd command. You'd do something like this:

```
echo 'pi:newpassword' | chpasswd # change user pi password to newpassword
```

Another alternative is to use the yes command in your script.
```
yes newpassword | passwd youruser
```