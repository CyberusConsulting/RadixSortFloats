@echo off

git add -A ./
git commit -a -m %1
git push -u origin main
