import java.util.Scanner;

public class Student extends User {
    int id;
    String Class;

    public Student(String name, String pass, int id, String Class) {
        super(name, pass);
        this.id = id;
        this.Class = Class;
    }

    public int login() {
        System.out.println("请输入学号：");
        Scanner sc = new Scanner(System.in);
        int id = sc.nextInt();
        System.out.println("请输入密码：");
        String pass = sc.next();

        if (id == this.id && pass.equals(getPass())) {
            System.out.println(Class + " " + name + "你好");
            return 1;
        }

        return 0;
    }

    public void show() {
        System.out.println(id + " " + name + " " + Class);
    }
}
