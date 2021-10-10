import java.util.Scanner;

public class Teacher extends User{
    int workId;
    String level;

    public Teacher(String name, String pass, int workdId, String level) {
        super(name, pass);
        this.workId = workdId;
        this.level = level;
    }

    public int login() {
        System.out.println("请输入工号：");
        Scanner sc = new Scanner(System.in);
        int id = sc.nextInt();
        System.out.println("请输入密码：");
        String pass = sc.next();
        if (id == this.workId && pass.equals(getPass())) {
            System.out.print(name + " " + level + " 您好");
            return 1;
        }

        return 0;
    }

    public void show() {
        System.out.println(workId + " " + name);
    }
}
