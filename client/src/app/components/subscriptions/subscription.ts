import { Component, DestroyRef, OnInit, computed, inject, signal } from '@angular/core';
import { takeUntilDestroyed } from '@angular/core/rxjs-interop';
import { MatButtonModule } from '@angular/material/button';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';

import { FeedSubscription } from '@models/subscription';
import { SubscriptionService } from '@services/subscription/subscription';
import { HeaderComponent } from "@components/header/header";
import { TimeService } from '@services/time/time';


@Component({
    selector: 'app-subscription-page',
    standalone: true,
    imports: [
    MatFormFieldModule,
    MatInputModule,
    MatIconModule,
    MatButtonModule,
    MatProgressSpinnerModule,
    HeaderComponent
],
    templateUrl: './subscription.html',
    styleUrl: './subscription.scss',
})
export class SubscriptionPage implements OnInit {
    private readonly m_subscriptionService = inject(SubscriptionService);
    private readonly m_destroyRef = inject(DestroyRef);
    private readonly m_committed = signal<FeedSubscription[]>([]);
    readonly draft = signal<FeedSubscription[]>([]);
    readonly loading = signal(true);
    readonly loadError = signal<string | null>(null);
    readonly saving = signal(false);
    readonly saveError = signal<string | null>(null);

    readonly newHost = signal('');
    readonly newEndpoint = signal('');
    readonly addError = signal<string | null>(null);

    readonly editingIndex = signal<number | null>(null);
    readonly editHost = signal('');
    readonly editEndpoint = signal('');
    readonly editError = signal<string | null>(null);

    readonly isDirty = computed(() => !this.subscriptionsEqual(this.draft(), this.m_committed()));

    constructor() {}

    public ngOnInit(): void {
        this.load();
    }

    public addSubscription(): void {
        const url = this.newHost().trim();
        if (!url) {
            this.addError.set('URL is required');
            return;
        }

        const { host, endpoint } = this.m_subscriptionService.parseURL(url);

        if (this.draft().some((s) => s.host === host && s.endpoint === endpoint)) {
            this.addError.set('That feed is already in the list.');
            return;
        }

        this.draft.update((list) => [...list, { host, endpoint, created_at: -1 }]);
        this.newHost.set('');
        this.newEndpoint.set('');
        this.addError.set(null);
    }

    public cancelEdit(): void {
        this.editingIndex.set(null);
        this.editError.set(null);
    }

    public confirmEdit(): void {
        const index = this.editingIndex();
        if (index === null) return;

        const host = this.editHost().trim();
        const endpoint = this.editEndpoint().trim();

        if (!host || !endpoint) {
            this.editError.set('Host and endpoint are both required.');
            return;
        }
        if (this.draft().some((s, i) => i !== index && s.host === host && s.endpoint === endpoint)) {
            this.editError.set('That feed is already in the list.');
            return;
        }

        this.draft.update((list) =>
            list.map((subscription, i) => (i === index ? { ...subscription, host, endpoint } : subscription)),
        );
        this.cancelEdit();
    }

    public discardChanges(): void {
        this.draft.set(this.cloneAll(this.m_committed()));
        this.cancelEdit();
        this.addError.set(null);
    }

    public load(): void {
        this.loading.set(true);
        this.loadError.set(null);

        this.m_subscriptionService
            .fetchSubscriptions()
            .pipe(takeUntilDestroyed(this.m_destroyRef))
            .subscribe({
                next: (subscriptions) => {
                    this.m_committed.set(this.cloneAll(subscriptions));
                    this.draft.set(this.cloneAll(subscriptions));
                    this.loading.set(false);
                },
                error: () => {
                    this.loadError.set('Could not load your subscribed feeds.');
                    this.loading.set(false);
                },
            });
    }

    public removeSubscription(index: number): void {
        if (this.editingIndex() === index) this.cancelEdit();
        this.draft.update((list) => list.filter((_, i) => i !== index));
    }

    public saveChanges(): void {
        if (!this.isDirty() || this.saving()) return;

        this.saving.set(true);
        this.saveError.set(null);
        const toSave = this.cloneAll(this.draft());

        this.m_subscriptionService
            .saveSubscriptions(toSave)
            .pipe(takeUntilDestroyed(this.m_destroyRef))
            .subscribe({
                next: () => {
                    // Keep the local values (including any 'Just now' placeholders) rather than the
                    // server response — real timestamps only show up after the next full page load.
                    this.m_committed.set(toSave);
                    this.draft.set(this.cloneAll(toSave));
                    this.saving.set(false);
                },
                error: () => {
                    this.saveError.set('Could not save your changes.');
                    this.saving.set(false);
                },
            });
    }

    public startEdit(index: number): void {
        const subscription = this.draft()[index];
        this.editingIndex.set(index);
        this.editHost.set(subscription.host);
        this.editEndpoint.set(subscription.endpoint);
        this.editError.set(null);
    }

    public timeAgo(timestampSeconds: number): string {
        return TimeService.timeAgo(timestampSeconds);
    }

    public updateEditEndpoint(value: string): void {
        this.editEndpoint.set(value);
    }

    public updateEditHost(value: string): void {
        this.editHost.set(value);
    }

    public updateNewHost(value: string): void {
        this.newHost.set(value);
    }

    private cloneAll(subscriptions: FeedSubscription[]): FeedSubscription[] {
        return subscriptions.map((subscription) => ({ ...subscription }));
    }

    private subscriptionsEqual(a: FeedSubscription[], b: FeedSubscription[]): boolean {
        if (a.length !== b.length)
            return false;

        return a.every((subscription, i) => subscription.host === b[i].host &&
                                            subscription.endpoint === b[i].endpoint &&
                                            subscription.created_at === b[i].created_at);
    }
}