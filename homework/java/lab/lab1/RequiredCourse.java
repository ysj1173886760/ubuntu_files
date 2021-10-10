public class RequiredCourse extends Course {
    private int credit;
    
    public RequiredCourse(int i, String n) {
        this.id = i;
        this.name = n;
        type = 0;
    }

    public void show() {
        System.out.println(id + " " + name + " 必修 " + teacher + " " + stuNum + " " + credit);
    }

    public RequiredCourse(int id, String name, int type, int stuNum, String teacher, int credit) {
        super(id, name, type, stuNum, teacher);
        this.credit = credit;
    }

    public String toString() {
        return super.toString() + " " + this.credit + "\n";
    }
}
