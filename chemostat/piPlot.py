import ftplib

with ftplib.FTP('10.42.0.139', 'pi', 'stlabpi') as ftp:
    ftp.nlst()