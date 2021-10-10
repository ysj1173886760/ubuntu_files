import java.util.Scanner;

public class User {
    String name;
    private String pass;

    public User(String name, String pass) {
        this.name = name;
        this.pass = pass;
    }

    public String getPass() {
        return pass;
    }

    public void setPass(String pass) {
        if (pass.length() == 6) {
            this.pass = pass;
        }
    }

    public User() {}

    public int login() {
        System.out.println("请输入管理员密码：");
        Scanner sc = new Scanner(System.in);

        if (sc.next().equals(this.pass)) {
            return 1;
        }

        return 0;
    }

    public void show() {
        System.out.println("用户名：" + name);
    }

    public void setPassWord() {
        Scanner sc = new Scanner(System.in);
        while (true) {
            System.out.print("请输入新密码（6位）：");
            String xpass1 = sc.next();
            System.out.print("请再次输入密码（6位）：");
            String xpass2 = sc.next();
            if (xpass1.equals(xpass2) && xpass1.length() == 6) {
                this.pass = xpass1;
                break;
            }
        }
    }
}
