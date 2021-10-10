public class OptionalCourse extends Course {
    private int maxStuNum;

    public OptionalCourse(int i, String s) {
        this.id = i;
        this.name = s;
        type = 1;
    }

    public void show() {
        System.out.print(id);
        System.out.print(" " + name);
        System.out.println(" 选修 " + teacher + " " + stuNum + " " + maxStuNum);
    }

    public OptionalCourse(int id, String name, int type, int stuNum, String teacher, int maxStuNum) {
        super(id, name, type, stuNum, teacher);
        this.maxStuNum = maxStuNum;
    }

    public String toString() {
        return super.toString() + " " + this.maxStuNum + "\n";
    }
}
