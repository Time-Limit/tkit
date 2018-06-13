---
title: 信号：基本概念
date: 2018-06-12 21:56:02
tags:
---

## 信号：基本概念

* 各种不同信号及其用途。
* 内核可能为进程产生信号的环境，以及某一进程向另一进程发送信号所使用的系统调用。
* 进程在默认情况下对信号的响应方式。以及进程改变响应信号方式的手段，特别是借助于信号处理器程序的手段，即程序受到信号时去自动调用的函数，由程序员定义。
* 借助进程信号掩码来阻塞信号，以及等待信号的相关概念。
* 如何暂定进程的执行，并等待信号的等待。

### 概念和概述

信号是事件发生时对进程的通知机制。有时已成为软件中断。信号与硬件中断的相似之处在于打断了程序执行的正常流程，大多数情况下，无法预测信号到达的精确时间。

一个具有合适权限的进程可以向另一个进程发送信号。进程亦可向自己发送信号。然而，发往进程的诸多信号，通常源于内核。引发内核为进程产生信号的各类事件如下：

* 硬件发生一场。比如，除零，内存访问越界。
* 用户键入了能够产生信号的终端特殊字符。如，CTRL+C(中断字符)，CTRL+Z(暂停字符)。
* 发生了软件时间。如，进程执行的CPU时间超限，文件描述符的输出变为有效。

针对每个信号，都定义了一个唯一的整数，从 1 开始展开。<signal.h> 以 SIG_NAME 格式的符号名对这些整数做了定义。