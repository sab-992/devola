import { Subscriber } from "@classes/subscriber/subscriber";


type UpdateCallback = (data: object) => void;

export class GenericSubscriber extends Subscriber {
    private m_callback: UpdateCallback;

    public static create(callback: UpdateCallback): GenericSubscriber {
        return new GenericSubscriber(callback);
    };

    private constructor(callback: UpdateCallback) {
        super();

        this.m_callback = callback;
    }

    public update(data: object): void {
        this.m_callback(data);
    }
}